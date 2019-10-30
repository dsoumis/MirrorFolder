#include "operations_in_tree.h"
#include <libgen.h>
void synchronizeRec(tree **Btree,tree *STnode,tree **BTnode,string Spath,string Bpath,i_node **Blist,i_node *Slist,mode m){
        char temp[256],name[256];
        tree *tr;
        if (STnode==NULL && *BTnode==NULL) return;
        if(STnode==NULL) {  /* BTnode has one or more uncessary files/directories in this path remove it all */
                synchronizeRec(Btree,STnode,&((*BTnode)->next),Spath,Bpath,Blist,Slist,m); /* remove it if it is a file but if it is a folder first remove all subfolders and files */
                strcpy(name,(*BTnode)->name);
                strcpy(temp,Spath);
                strcat(temp,basename(name));
                strcpy(name,(*BTnode)->name);
                if(m==file) d(temp,name,Btree,Blist,Slist);
                else b(temp,name,Btree,Blist,Slist);
                return;
        }
        else if(*BTnode==NULL) {
                strcpy(temp,Bpath);
                strcpy(name,STnode->name);
                strcat(temp,basename(name));
                if(STnode->m==file) *BTnode=c(STnode->name,temp,STnode->inode,Btree,Blist,Slist);
                else *BTnode=a(STnode->name,temp,Btree,Blist,Slist);
                STnode->inode->destination=(*BTnode)->inode;


        } /*STnode has a file/directory that *BTnode has not so add it */
        else {
                strcpy(temp,STnode->name);
                strcpy(name,(*BTnode)->name);;
                if(strcmp(basename(name),basename(temp))==0) {
                        if(m==file) {

                                strcpy(name,(*BTnode)->name);
                                e(STnode->name,name,STnode->inode,&((*BTnode)->inode),Btree,Blist,Slist);
                        }  /* same name check if they are files and if so check if their inodes match */
                        else {
                          STnode->inode->destination=(*BTnode)->inode;
                        }
                }
                else { /* not same name,so BTnode has an uncessary file/dir and we must add its respective from Stree */
                        strcpy(name,(*BTnode)->name);
                        strcpy(temp,Spath);
                        strcat(temp,basename(name));
                        strcpy(name,(*BTnode)->name);
                        if(m==file) d(temp,name,Btree,Blist,Slist);
                        else b(temp,name,Btree,Blist,Slist);
                        strcpy(temp,Bpath);
                        strcpy(name,STnode->name);
                        strcat(temp,basename(name));
                        if(m==file) tr=c(STnode->name,temp,STnode->inode,Btree,Blist,Slist);
                        else tr=a(STnode->name,temp,Btree,Blist,Slist);
                        STnode->inode->destination=(tr)->inode;
                        strcpy(temp,STnode->name);
                        strcpy(name,(*BTnode)->name);;
                        if(strcmp(basename(name),basename(temp))!=0) {
                                synchronizeRec(Btree,STnode->next,BTnode,Spath,Bpath,Blist,Slist,m);
                                if(m==directory) {
                                        synchronizeRec(Btree,STnode->next,&((*BTnode)->next),Spath,Bpath,Blist,Slist,m);
                                        synchronizeRec(Btree,STnode->directories,&((*BTnode)->directories),STnode->name,(*BTnode)->name,Blist,Slist,directory);
                                        synchronizeRec(Btree,STnode->files,&((*BTnode)->files),STnode->name,(*BTnode)->name,Blist,Slist,file);
                                }
                                return;
                        } /* new file/directory was not put in the same tree node so we must delete this backup node tree before moving to the next and if it's a directory to its subfolders and files */
                }
        }
        synchronizeRec(Btree,STnode->next,&((*BTnode)->next),Spath,Bpath,Blist,Slist,m);
        if(m==directory) {
                synchronizeRec(Btree,STnode->directories,&((*BTnode)->directories),STnode->name,(*BTnode)->name,Blist,Slist,directory);
                synchronizeRec(Btree,STnode->files,&((*BTnode)->files),STnode->name,(*BTnode)->name,Blist,Slist,file);
        }
      /* sync the next node and if it's a directory its subfolders and files */
}
void synchronize(tree **Btree,tree *Stree,i_node **Blist,i_node *Slist){
        synchronizeRec(Btree,Stree->directories,&((*Btree)->directories),Stree->name,(*Btree)->name,Blist,Slist,directory); /* compare and sychronize all subfolders and files */
        synchronizeRec(Btree,Stree->files,&((*Btree)->files),Stree->name,(*Btree)->name,Blist,Slist,file); /* compare and sychronize all subfolders and files */
}
