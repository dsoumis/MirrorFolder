#include "operations_in_tree.h"
#include <unistd.h>
#include <stdio.h>
#include "copy_function.h"
#include <libgen.h>
tree *a(char *path_source,char *path_destination,tree **Btree,i_node **Blist,i_node *Slist) {
        struct stat info_dest, info_source;
        char temp_name[256],temp_path[256];
        strcpy(temp_name,path_destination);
        strcpy(temp_path,dirname(path_destination));
        strcat(temp_path,"/");
        //If name doesn't exist in the backup tree and exists in source
        if (stat(temp_name, &info_dest) == -1 &&stat(path_source, &info_source) != -1) {
                if (mkdir(temp_name, S_IRWXU | S_IRWXG | S_IRWXO) != 0) {//Create the directory
                        perror("Error in creating directory\n");
                        exit(1);
                }
                stat(temp_name,&info_dest);
                strcat(temp_name,"/");
                *Btree=insert(*Btree,temp_path,temp_name,directory,Blist,info_dest.st_ino,info_dest.st_mtime,info_dest.st_size);
                return find(*Btree,temp_path,temp_name,directory);
        } else if (stat(temp_name, &info_dest) != -1 &&
                   stat(path_source, &info_source) != -1) { //If the name exists both in destination and source
                if ((info_dest.st_mode & S_IFMT) != S_IFDIR) {//..and it is not a directory but a file
                        if (unlink(temp_name) != 0) {//unlinking the file
                                perror("Error in deleting file\n");
                                exit(1);
                        }
                        if (mkdir(temp_name, S_IRWXU | S_IRWXG | S_IRWXO) != 0) {//Create the directory
                                perror("Error in creating directory\n");
                                exit(1);
                        }

                        delete(Btree,temp_path,temp_name,Blist,Slist,file);
                        stat(temp_name,&info_dest);
                        strcat(temp_name,"/");
                        *Btree=insert(*Btree,temp_path,temp_name,directory,Blist,info_dest.st_ino,info_dest.st_mtime,info_dest.st_size);
                        return find(*Btree,temp_path,temp_name,directory);
                }
                strcat(temp_name,"/");
                return find(*Btree,temp_path,temp_name,directory);
        }
        strcat(temp_name,"/");
        return find(*Btree,temp_path,temp_name,directory);
}
void recursion_for_b(char* name){
        struct stat info;
        DIR *dir_pointer;
        struct dirent *direntp;
        if ((dir_pointer = opendir(name)) == NULL) {
                perror("Error in opening directory\n");
                exit(1);
        }
        char temp[100];
        strcpy(temp, name);
        while ((direntp = readdir(dir_pointer)) != NULL) {//Delete first all files in the directory
                if(strcmp(direntp->d_name,".")==0||strcmp(direntp->d_name,"..")==0)
                        continue;
                strcat(temp,"/");
                strcat(temp, direntp->d_name);
                stat(temp, &info);
                if ((info.st_mode & S_IFMT) == S_IFDIR) //If there is another directory
                        recursion_for_b(temp); //recursively delete the directories
                else if((info.st_mode & S_IFMT) != S_IFDIR) {
                        if (unlink(temp) != 0)
                        {
                                perror("Error in deleting file\n");
                                exit(1);
                        }
                }
                strcpy(temp, name);
        }
        closedir(dir_pointer);
        if (rmdir(temp) != 0) {//Then delete the directory
                perror("Error in deleting directory\n");
                exit(1);
        }
}
void b(char *path_source,char *path_destination,tree **Btree,i_node **Blist,i_node *Slist) { //works
        struct stat info_dest, info_source;
        int i;
        char temp_name[256],temp_path[256];
        strcpy(temp_name,path_destination);
        strcpy(temp_path,dirname(path_destination));
        strcat(temp_path,"/");
        if (stat(temp_name, &info_dest) != -1 && (stat(path_source,&info_source)==-1 || (info_source.st_mode & S_IFMT) != S_IFDIR)) { //If name exists in backup tree
                //and name is directory but doesnt exist in source tree
                if ((info_dest.st_mode & S_IFMT) == S_IFDIR) {
                        recursion_for_b(temp_name);
                        delete(Btree,temp_path,temp_name,Blist,Slist,directory);
                }
        }
}
tree *c(char *path_source,char *path_destination,i_node *Sinode,tree **Btree,i_node **Blist,i_node *Slist) {
        i_node *i;
        struct node *list;
        struct stat info_dest, info_source;
        char temp_path[256],temp_name[256],name[256];
        strcpy(temp_name,path_destination);
        strcpy(temp_path,dirname(path_destination));
        strcat(temp_path,"/");
        //If a file exists in source but doesn't exist in destination
        if (stat(path_source, &info_source) != -1 && stat(temp_name, &info_dest) == -1) {
                if ((info_source.st_mode & S_IFMT) != S_IFDIR) {
                        if((i=Sinode->destination)!=NULL) {
                                if(link(i->list_of_names->name,temp_name)!=0) {
                                        perror("Error in linking file\n");
                                        exit(1);
                                }
                        }
                        else {
                                creat(temp_name, S_IRWXU | S_IRWXG | S_IRWXO);
                                cp(temp_name,path_source);
                        }
                        stat(temp_name,&info_dest);
                        *Btree=insert(*Btree,temp_path,temp_name,file,Blist,info_dest.st_ino,info_dest.st_mtime,info_dest.st_size);
                        return find(*Btree,temp_path,temp_name,file);
                }
                return find(*Btree,temp_path,temp_name,file);
        }
        printf("%s\n",temp_name);
        return find(*Btree,temp_path,temp_name,file);
}
void d(char *path_source,char *path_destination,tree **Btree,i_node **Blist,i_node *Slist) { //works. give path /home/user/ etc..
        struct stat info_dest,info_source;
        char temp_path[256],temp_name[256];
        strcpy(temp_name,path_destination);
        strcpy(temp_path,dirname(path_destination));
        strcat(temp_path,"/");
        //If a file doesn't exist in source but exists in destination
        if ( stat(temp_name, &info_dest) != -1 && stat(path_source,&info_source)==-1) {
                if ((info_dest.st_mode & S_IFMT) != S_IFDIR) {
                        if (unlink(temp_name) != 0) {//unlink the file
                                perror("Error in deleting file\n");
                                exit(1);
                        }
                        delete(Btree,temp_path,temp_name,Blist,Slist,file);
                }
        }
}

void e(char *path_source,char *path_destination,i_node *Sinode,i_node **Binode,tree **Btree,i_node **Blist,i_node *Slist){
        struct stat info_dest, info_source;
        tree *tr;
        char temp_path[256],temp_name[256];
        strcpy(temp_name,path_destination);
        strcpy(temp_path,dirname(path_destination));
        strcat(temp_path,"/");;
        //If a file exists in source and in destination
        if (stat(path_source, &info_source) != -1 && stat(temp_name, &info_dest) != -1) {
                if (((info_dest.st_mode & S_IFMT) != S_IFDIR) && ((info_source.st_mode & S_IFMT) != S_IFDIR)) {
                        if(Sinode->destination==*Binode) return; /* no deletes needed since they are connected with their inodes */
                        else{  /* source file is not linked with backup */
                                if (unlink(temp_name) != 0)
                                {
                                        perror("Error in deleting file\n");
                                        exit(1);
                                }
                                delete(Btree,temp_path,temp_name,Blist,Slist,file);
                                tr=c(path_source,temp_name,Sinode,Btree,Blist,Slist); /* get the necessary info for the synchronize function */
                                Sinode->destination=tr->inode; /* connect their inodes */
                        }
                }
        }
}
