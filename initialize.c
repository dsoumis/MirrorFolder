#include "initialize.h"
void initializeRec(DIR *dir,char *path,tree **t,i_node **list){
        DIR *d2;
        struct dirent *d;
        tree *tr;
        struct stat s;
        int i,j;
        char temp[256];
        while(d=readdir(dir)) { /* read directory's files and subdirectories */
                strcpy(temp,path);
                strcat(temp,d->d_name);
                if(stat(temp,&s)==-1) {
                        perror("Cannot open file/directory\n");
                        exit(1);
                }
                tr=*t;
                if(S_ISREG(s.st_mode)) {
                        *t=insert(tr,path,temp,file,list,s.st_ino,s.st_size,s.st_mtime);
                }
                else {
                        if((temp[strlen(temp)-1]=='.' && temp[strlen(temp)-2]=='/') || (temp[strlen(temp)-1]=='.' && temp[strlen(temp)-2]=='.')) {
                                continue;
                        } /* by-pass current directory name and previous directory name */
                        strcat(temp,"/");
                        *t=insert(tr,path,temp,directory,list,s.st_ino,s.st_size,s.st_mtime); /* insert to the tree */
                        if((d2=opendir(temp))==NULL){
                                perror("Error in opening directory\n");
                                exit(1);
                        }
                        initializeRec(d2,temp,t,list); /* find the files of its subdirectory */
                }
        }
        closedir(dir);
}
void initialize(char *path,tree **t,i_node **list){ /* open the first directory and then recursively add all subfolders and files to the tree */
        DIR *dir;
        i_node *inode;
        struct stat s;
        if((dir=opendir(path))==NULL){
                perror("Error in opening directory\n");
                exit(1);
        }
        (*t)=malloc(sizeof(tree));
        strcpy((*t)->name,path);
        stat(path,&s);
        (*t)->m=directory;
        (*t)->files=NULL;
        (*t)->directories=NULL;
        (*t)->next=NULL;
        inode=create_inode(s.st_ino,s.st_size,s.st_mtime,(*t)->name);
        *list=list_insert(*list,inode);
        (*t)->inode=inode;
        (*t)->inode->destination=NULL; /* first node is always the head directory */
        initializeRec(dir,path,t,list);
}
