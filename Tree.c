#include "Tree.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
tree *insert(tree *t,string path,string name,mode m,i_node **list,ino_t ino,time_t mtime,off_t size){
        tree *previous,*next,*temp;
        i_node *inode;
        if (t==NULL) return NULL;
        if(!strcmp(path,t->name)) { /* current path is the goal path */
                if(m==directory) { /* we insert a directory */
                        if(t->directories==NULL) temp=NULL;
                        else if(strcmp(t->directories->name,name)>0) temp=t->directories;
                        else {
                                next=t->directories;
                                while(next!=NULL && strcmp(next->name,name)<0) {                                                    /* sort this file/directory correctly */
                                        previous=next;
                                        next=next->next;
                                }
                                if((previous->next=malloc(sizeof(tree)))==NULL) {
                                        perror("Error in allocating dynamic memory\n");
                                        exit(1);
                                }
                                strcpy(previous->next->name,name);
                                previous->next->files=NULL;
                                previous->next->directories=NULL;
                                previous->next->m=m;
                                previous->next->next=next;
                                if((previous->next->inode=check_list(*list,ino,previous->next->name))==NULL) {
                                        previous->next->inode=create_inode(ino,size,mtime,previous->next->name);
                                        *list=list_insert(*list,previous->next->inode);
                                } /* if inode does not exist create one and insert it int he list */
                                return t;
                        }
                        if((t->directories=malloc(sizeof(tree)))==NULL) {
                                perror("Error in allocating dynamic memory\n");
                                exit(1);
                        }
                        strcpy(t->directories->name,name);
                        t->directories->directories=NULL;
                        t->directories->files=NULL;
                        t->directories->next=temp;
                        t->directories->m=m;
                        if((t->directories->inode=check_list(*list,ino,t->directories->name))==NULL) {
                                t->directories->inode=create_inode(ino,size,mtime,t->directories->name);
                                *list=list_insert(*list,t->directories->inode);
                        }
                        return t;
                }
                else { /* we insert a file */
                        if(t->files==NULL) temp=NULL;
                        else if(strcmp(t->files->name,name)>0) temp=t->files;
                        else {
                                next=t->files;
                                while(next!=NULL && strcmp(next->name,name)<0) {                                                    /* sort this file/directory correctly */
                                        previous=next;
                                        next=next->next;
                                }
                                if((previous->next=malloc(sizeof(tree)))==NULL) {
                                        perror("Error in allocating dynamic memory\n");
                                        exit(1);
                                }
                                strcpy(previous->next->name,name);
                                previous->next->files=NULL;
                                previous->next->directories=NULL;
                                previous->next->m=m;
                                previous->next->next=next;
                                if((previous->next->inode=check_list(*list,ino,previous->next->name))==NULL) {
                                        previous->next->inode=create_inode(ino,size,mtime,previous->next->name);
                                        *list=list_insert(*list,previous->next->inode);
                                } /* if inode does not exist create one and insert it int he list */
                                return t;
                        }
                        if((t->files=malloc(sizeof(tree)))==NULL) {
                                perror("Error in allocating dynamic memory\n");
                                exit(1);
                        }
                        strcpy(t->files->name,name);
                        t->files->directories=NULL;
                        t->files->files=NULL;
                        t->files->next=temp;
                        t->files->m=m;
                        if((t->files->inode=check_list(*list,ino,t->files->name))==NULL) {
                                t->files->inode=create_inode(ino,size,mtime,t->files->name);
                                *list=list_insert(*list,t->files->inode);
                        }
                        return t;
                }
        }
        else {
                next=t->directories;
                while(next!=NULL) { /* move to the right parent-directory in order to get to the goal path */
                        if(strstr(path,next->name)!=NULL) {
                                if((temp=insert(next,path,name,m,list,ino,size,mtime))==NULL) { /* did not find the path so retry at a next directory */
                                        next=next->next;
                                        continue;
                                }
                                next=temp;
                                return t; /* found directory return current directory */
                        }
                        else {
                                next=next->next; /* go the the next directory to find the file's path */
                        }
                }
        }
        return NULL;  /* didn't find a result in this path */
}
void deleteRec(tree **t,i_node **list,i_node *Slist,int c){ /* first delete all the siblings,then its childs and finally the node itself */
        if (*t==NULL) return;
        if(c!=0) deleteRec(&((*t)->next),list,Slist,1);
        deleteRec(&((*t)->directories),list,Slist,1);
        deleteRec(&((*t)->files),list,Slist,1);
        *list=rm_inode_list(*list,(*t)->inode->inode_id,(*t)->name,Slist);
        free(*t);
}
void tree_modify(tree *t,char *name,ino_t ino,time_t mtime,off_t size,i_node **list,i_node *Slist){ /* modify the tree node based on the inode given */
        *list=rm_inode_list(*list,t->inode->inode_id,t->name,Slist);
        strcpy(t->name,name);
        if((t->inode=check_list(*list,ino,t->name))==NULL) {
                t->inode=create_inode(ino,size,mtime,t->name);
                *list=list_insert(*list,t->inode);
        } /* remove current inode and attach the node to a new one */
}
tree *delete(tree **t,string path,string name,i_node **list,i_node *Slist,mode m){
        tree *next,*temp;
        if(t==NULL) return NULL;
        if(strcmp((*t)->name,path)==0) { /* current path is the goal path,now we have to find the file/directory and delete it */
                if (m==file) { /* we search for a file */
                        if(strcmp((*t)->files->name,name)==0) {
                                temp=(*t)->files->next;
                                deleteRec(&(*t)->files,list,Slist,0);
                                (*t)->files=temp;
                                return *t;

                        }
                        next=(*t)->files;
                }
                else { /* we search for a directory */
                        if(strcmp((*t)->directories->name,name)==0) {
                                temp=(*t)->directories->next;
                                deleteRec(&(*t)->directories,list,Slist,0);
                                (*t)->directories=temp;
                                return *t;
                        }
                        next=(*t)->directories;
                }
                while(strcmp(next->next->name,name)!=0) {
                        next=next->next;
                } /* loop until the next name is the desired name */
                temp=next->next->next;
                deleteRec(&(next->next),list,Slist,0);
                next->next=temp;
                return *t;
        }
        else {
                next=(*t)->directories;
                while(next!=NULL) {  /* move to the right directory in order to get to the goal path */
                        if(strstr(path,next->name)!=NULL)  {
                                if((temp=delete(&next,path,name,list,Slist,m))==NULL) { /* did not find the path so retry at a next directory */
                                        next=next->next;
                                        continue;
                                }
                                return *t;
                        }
                        else next=next->next;
                }
        }
        return NULL;  /* didn't find a result in this path */
}
tree *find(tree *t,string path,string name,mode m){
        tree *next,*temp;
        if(t==NULL) return NULL;
        if(strcmp(t->name,path)==0) {
                if (m==file) { /* we search for a directory */
                        if(strcmp(t->files->name,name)==0) return t->files;
                        next=t->files;
                }
                else { /* we search for a directory */
                        if(strcmp(t->directories->name,name)==0) return t->directories;
                        next=t->directories;
                }
                while(strcmp(next->next->name,name)!=0) {
                        next=next->next;
                } /* loop until the next name is the desired name */
                return next->next;
        }
        else {
                next=t->directories;
                while(next!=NULL) {  /* move to the right directory in order to get to the goal path */
                        if(strstr(path,next->name)!=NULL)  {
                                if((temp=find(next,path,name,m))==NULL) {
                                        next=next->next;
                                        continue;
                                }
                                return temp; /* found the directory return the node that was found */
                        }
                        else next=next->next; /* go the the next directory to find the file's path */
                }
        }
        return NULL; /* didn't find a result in this path */
}
void directories_traverse(tree *t,int *counter,char **paths){ /* simple traverse to find all the directories path */
        if (t==NULL) return;
        strcpy(paths[*counter],t->name);
        (*counter)++;
        directories_traverse(t->next,counter,paths);
        directories_traverse(t->directories,counter,paths);
}
void print_traverse(tree *t,int i) { /* simple traverse of the tree print he node itself then the subdirectories and files(if the current node is a directory) and the siblings (+their levels) */
        if (t==NULL) return;
        printf("Level : %d\t\t\tName : %s\n",i,t->name);
        if(t->m==directory){
                print_traverse(t->directories,i+1);
                print_traverse(t->files,i+1);
        }
        print_traverse(t->next,i);
}
