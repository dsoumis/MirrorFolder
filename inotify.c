#include "inotify.h"
#include <signal.h>

int fd;
tree **BT,**ST;
i_node **SL,**BL;
char **DP,**SP;
void inthand(int signum)
{
    printf("\nSignal to terminate!\n");
    printf("\n\n\n\n\t\t\t\t\t\t\t\t\tSOURCE TREE :\n\n\n\n");
    print_traverse(*ST,1);
    printf("\n\n\n\n\t\t\t\t\t\t\t\t\tBACKUP TREE :\n\n\n\n");
    print_traverse(*BT,1); /* print the final trees */
    printf("\n\n\n\n");
    deleteRec(ST,SL,NULL,0);
    deleteRec(BT,BL,*SL,0);
    for(int i=0; i<256; i++) {
        free(SP[i]);
        free(DP[i]);
    }
    free(SP);
    free(DP); /* free tree and directories' name*/
    close(fd);
    exit(0);
}
//The fixed size of the event buffer:
#define EVENT_SIZE  ( sizeof (struct inotify_event) )

//The size of the read buffer: estimate 1024 events with 16 bytes per name over and above the fixed size above
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
const char * target_type(struct inotify_event *event) ;
const char * target_name(struct inotify_event *event);
const char * event_name(struct inotify_event *event);
void function(i_node **Blist,i_node **Slist,tree **Btree,tree **Stree,int number_of_source_directories,char** paths_of_source_directories,char** paths_of_destination_directories){
    char temp[100],path[256],temp2[100];
    int length, read_ptr, read_offset; //management of variable length events
    int watched;
    char buffer[EVENT_BUF_LEN];	//the buffer to use for reading the events
    int wd;
    struct stat info;
    i_node *temp_inode;
    char previous_event_name[100],previous_event_target_name[100],previous_event_target_type[100];
    strcpy(previous_event_name,"");
    strcpy(previous_event_target_name,"");
    strcpy(previous_event_target_type,"");
    char modify[100];
    unsigned int cookie=0;
    char cookie_path[100],cookie_source[100];
    tree *Btr,*Str;
    SP=paths_of_source_directories;
    DP=paths_of_destination_directories;
    BT=Btree;
    ST=Stree;
    BL=Blist;
    SL=Slist;
    fd = inotify_init();
    if (fd < 0)
        perror("inotify_init");

    watched = 0;
    for(int i=0; i<number_of_source_directories; i++){
        /*adding a watch for each directory in source tree*/
        wd = inotify_add_watch(fd, paths_of_source_directories[i], IN_ALL_EVENTS);
        if( wd == -1 ) {
            printf("failed to add watch %s\n", paths_of_source_directories[i]);
        } else {
            printf("Watching %s as %d\n", paths_of_source_directories[i], wd);
            watched++;
        }
    }

    if( watched == 0 )
        perror("Nothing to watch!");

    read_offset = 0; //remaining number of bytes from previous read

    signal(SIGINT, inthand);
    while (1) {
        /* read next series of events */
        length = read(fd, buffer + read_offset, sizeof(buffer) - read_offset);
        if (length < 0)
            perror("read");
        length += read_offset; // if there was an offset, add it to the number of bytes to process
        read_ptr = 0;

        // process each event
        // make sure at least the fixed part of the event is included in the buffer
        while (read_ptr + EVENT_SIZE <= length ) {
            //point event to beginning of fixed part of next inotify_event structure
            struct inotify_event *event = (struct inotify_event *) &buffer[ read_ptr ];

            // if however the dynamic part exceeds the buffer,
            // that means that we cannot fully read all event data and we need to
            // deffer processing until next read completes
            if( read_ptr + EVENT_SIZE + event->len > length )
                break;
            //event is fully received, process
            printf("WD:%d %s %s %s COOKIE=%u\n", event->wd, event_name(event), target_type(event), target_name(event), event->cookie);
            if(strcmp(event_name(event),"watch target deleted")==0) { //works
                strcpy(temp, paths_of_destination_directories[event->wd - 1]);
                recursion_for_b(temp); //Delete the directory and its elements

                strcpy(temp2, paths_of_source_directories[event->wd - 1]); //source path that was just deleted
                strcpy(path,dirname(temp2));
                strcpy(temp2, paths_of_source_directories[event->wd - 1]);
                strcat(path,"/");
                delete(Stree,path,temp2,Slist,NULL,directory);
                strcpy(temp, paths_of_destination_directories[event->wd - 1]);
                strcpy(path,dirname(temp));
                strcpy(temp, paths_of_destination_directories[event->wd - 1]);
                strcat(path,"/");
                delete(Btree,path,temp,Blist,*Slist,directory);
                /* delete directory from both trees */
                inotify_rm_watch(fd, wd); //Remove wd
            }
            if(event->len!=0 && //If it's not the previous event
            (strcmp(previous_event_name,event_name(event))!=0 ||
            strcmp(previous_event_target_name,target_name(event))!=0 ||
            strcmp(previous_event_target_type,target_type(event))!=0))
            {

                if(strcmp(event_name(event),"create")==0){
                    int i;
                    if(event->mask & IN_ISDIR) {//works
                        //Each event in source is assigned to a wd beggining from 1. So the first path-0 is 1. Identically to destination.
                        strcpy(temp,paths_of_destination_directories[event->wd-1]);
                        strcat(temp,target_name(event));
                        strcat(temp,"/");
                        if(stat(temp,&info)==-1) { //If it doesn't already exist
                            if (mkdir(temp, S_IRWXU | S_IRWXG | S_IRWXO) != 0) {//Create the directory
                                perror("Error in creating directory\n");
                                exit(1);
                            }
                            i=number_of_source_directories++;

                            strcpy(paths_of_destination_directories[i],temp);

                            strcpy(path,dirname(temp));
                            strcpy(temp, paths_of_destination_directories[i]);
                            strcat(path,"/");
                            stat(temp,&info);
                            *Btree=insert(*Btree,path,temp,directory,Blist,info.st_ino,info.st_mtime,info.st_size);
                            Btr=find(*Btree,path,temp,directory);
                        }
                        strcpy(temp,paths_of_source_directories[event->wd-1]);
                        strcat(temp,target_name(event));
                        strcat(temp,"/");
                        strcpy(paths_of_source_directories[i],temp);
                        wd=inotify_add_watch(fd, temp, IN_ALL_EVENTS);
                        /* add new directory to inotify watch */
                        strcpy(path,dirname(temp));
                        strcpy(temp, paths_of_source_directories[i]);
                        strcat(path,"/");
                        stat(temp,&info);
                        *Stree=insert(*Stree,path,temp,directory,Slist,info.st_ino,info.st_mtime,info.st_size);
                        Str=find(*Stree,path,temp,directory);
                        Str->inode->destination=Btr->inode; /* insert the new directory to both trees and connect their inodes */
                    }else{
                        strcpy(temp,paths_of_source_directories[event->wd-1]);
                        strcpy(path,temp);
                        strcpy(temp,paths_of_source_directories[event->wd-1]);
                        strcat(temp,target_name(event));
                        stat(temp,&info);
                        *Stree=insert(*Stree,path,temp,file,Slist,info.st_ino,info.st_mtime,info.st_size);
                        Str=find(*Stree,path,temp,file);
                        strcpy(temp,paths_of_destination_directories[event->wd-1]);
                        strcat(temp,target_name(event));
                        if(Str->inode->destination!=NULL) {
                            link(Str->inode->destination->list_of_names->name,temp);
                        } /* link the new file as there's already a file in source */
                        else{
                            creat(temp, S_IRWXU | S_IRWXG | S_IRWXO);
                        } /* create a new file */
                        strcpy(temp, paths_of_destination_directories[event->wd-1]);
                        strcpy(path,temp);
                        strcpy(temp, paths_of_destination_directories[event->wd-1]);
                        strcat(temp,target_name(event));
                        stat(temp,&info);
                        *Btree=insert(*Btree,path,temp,file,Blist,info.st_ino,info.st_mtim.tv_sec,info.st_size);
                        Btr=find(*Btree,path,temp,file);
                        Str->inode->destination=Btr->inode; /* inser the new file to botch directories and connect their inodes */
                    }
                }else if(strcmp(event_name(event),"attrib")==0){
                    if(!(event->mask & IN_ISDIR)){//if it is a file

                        strcpy(temp2,paths_of_source_directories[event->wd-1]);
                        strcpy(path,temp2);
                        strcpy(temp2, paths_of_source_directories[event->wd-1]);
                        strcat(temp2,target_name(event));
                        stat(temp2,&info);
                        Str=find(*Stree,path,temp2,file);

                        if(info.st_mtim.tv_sec!=Str->inode->date_last_modified){ //If the time of modify in inode is different of the file's
                            tree_modify(Str,temp2,info.st_ino,info.st_mtim.tv_sec,info.st_size,Slist,NULL);
                            strcpy(temp, paths_of_destination_directories[event->wd-1]);
                            strcpy(path,temp);
                            strcpy(temp, paths_of_destination_directories[event->wd-1]);
                            strcat(temp,target_name(event));
                            cp(temp,temp2);
                            stat(temp,&info);
                            Btr=find(*Btree,path,temp,file);
                            tree_modify(Btr,temp,info.st_ino,info.st_mtim.tv_sec,info.st_size,Blist,*Slist);
                            Str->inode->destination=Btr->inode;

                        } /* modify file in both trees and connect their new inodes */
                    }
                }else if(strcmp(event_name(event),"modify")==0){//works
                    if(!(event->mask & IN_ISDIR)){ //If it is a file
                        strcpy(temp,paths_of_source_directories[event->wd-1]);
                        strcat(temp,target_name(event));
                        strcpy(modify,temp);//Save the path of modified file
                    }
                }else if(strcmp(event_name(event),"close write")==0){//works
                    strcpy(temp,paths_of_source_directories[event->wd-1]);
                    strcat(temp,target_name(event));
                    if(strcmp(modify,temp)==0){ //If it has been marked as modified
                        strcpy(temp2,paths_of_source_directories[event->wd-1]);
                        strcpy(path,temp2);
                        strcpy(temp2, paths_of_source_directories[event->wd-1]);
                        strcat(temp2,target_name(event));
                        stat(temp2,&info);
                        Str=find(*Stree,path,temp2,file);
                        tree_modify(Str,temp2,info.st_ino,info.st_mtim.tv_sec,info.st_size,Slist,NULL);
                        strcpy(temp, paths_of_destination_directories[event->wd-1]);
                        strcpy(path,temp);
                        strcpy(temp, paths_of_destination_directories[event->wd-1]);
                        strcat(temp,target_name(event));
                        cp(temp,modify);//copy it
                        stat(temp,&info);
                        Btr=find(*Btree,path,temp,file);
                        tree_modify(Btr,temp,info.st_ino,info.st_mtim.tv_sec,info.st_size,Blist,*Slist);
                        Str->inode->destination=Btr->inode;  /* modify file in both trees and connect their new inodes */

                    }
                }else if(strcmp(event_name(event),"delete")==0){//works
                    if(!(event->mask & IN_ISDIR)) {//If it is a file
                        strcpy(temp, paths_of_destination_directories[event->wd - 1]);
                        strcat(temp, target_name(event));
                        unlink(temp); //unlink it from destination


                        strcpy(temp2, paths_of_source_directories[event->wd - 1]); //source path that was just deleted
                        strcpy(path,temp2);
                        strcpy(temp2, paths_of_source_directories[event->wd - 1]);
                        strcat(temp2, target_name(event));
                        delete(Stree,path,temp2,Slist,NULL,file);

                        strcpy(path,dirname(temp));
                        strcpy(temp, paths_of_destination_directories[event->wd - 1]);
                        strcat(path,"/");
                        strcat(temp, target_name(event));
                        delete(Btree,path,temp,Blist,*Slist,file);
                        /* delete file from both trees */
                    }
                }else if(strcmp(event_name(event),"moved out")==0){//works
                    cookie=event->cookie; //note of the field cookie
                    strcpy(cookie_path, paths_of_destination_directories[event->wd - 1]);
                    strcat(cookie_path, target_name(event));

                    strcpy(cookie_source, paths_of_source_directories[event->wd - 1]);
                    strcat(cookie_source, target_name(event));
                }else if(strcmp(event_name(event),"moved into")==0){//works partly
                    if(event->cookie==cookie){
                        strcpy(temp,paths_of_destination_directories[event->wd-1]);
                        strcat(temp,target_name(event));
                        link(cookie_path,temp);//cut it
                        unlink(cookie_path);

                        strcpy(temp,paths_of_destination_directories[event->wd-1]);
                        strcpy(path,temp);
                        strcat(temp,target_name(event));
                        stat(temp,&info);
                        *Btree=insert(*Btree,path,temp,file,Blist,info.st_ino,info.st_mtime,info.st_size);
                        Btr=find(*Btree,path,temp,file);
                        strcpy(temp,cookie_path);
                        strcpy(path,dirname(temp));
                        strcpy(temp,cookie_path);
                        strcat(path,"/");

                        delete(Btree,path,temp,Blist,*Slist,file);

                        strcpy(temp,paths_of_source_directories[event->wd-1]);
                        strcpy(path,temp);
                        strcpy(temp,paths_of_source_directories[event->wd-1]);
                        strcat(temp,target_name(event));
                        stat(temp,&info);
                        *Stree=insert(*Stree,path,temp,file,Slist,info.st_ino,info.st_mtime,info.st_size);
                        Str=find(*Stree,path,temp,file);
                        strcpy(temp,cookie_source);
                        strcpy(path,dirname(temp));
                        strcpy(temp,cookie_source);
                        strcat(path,"/");
                        delete(Stree,path,temp,Slist,NULL,file);
                        Str->inode->destination=Btr->inode;


                        /* delete old file location and insert new file in both trees and connct their inodes */

                    }else{
                        strcpy(temp,paths_of_source_directories[event->wd-1]);
                        strcpy(path,temp);
                        strcat(temp,target_name(event));
                        stat(temp,&info);
                        *Stree=insert(*Stree,path,temp,file,Slist,info.st_ino,info.st_mtime,info.st_size);
                        Str=find(*Stree,path,temp,file);

                        strcpy(temp,paths_of_destination_directories[event->wd-1]);
                        strcat(temp,target_name(event));
                        if(Str->inode->destination!=NULL) {
                            link(Str->inode->destination->list_of_names->name,temp);
                        }
                        else{
                            creat(temp, S_IRWXU | S_IRWXG | S_IRWXO);
                        }
                        strcpy(path,temp);
                        strcat(path,"/");
                        strcat(temp,target_name(event));
                        stat(temp,&info);
                        *Btree=insert(*Btree,path,temp,file,Blist,info.st_ino,info.st_mtim.tv_sec,info.st_size);
                        Btr=find(*Btree,path,temp,file);
                        Str->inode->destination=Btr->inode;
                    } /* insert file in both directories and connect their inodes */
                }
                if(strcmp(event_name(event),"moved into")!=0 && strcmp(previous_event_name,"moved out")==0){//works
                    unlink(cookie_path);

                    strcpy(temp,cookie_source);
                    strcpy(path,dirname(temp));
                    strcpy(temp,cookie_source);
                    strcat(path,"/");
                    delete(Stree,path,temp,Slist,NULL,file);

                    strcpy(temp,cookie_path);
                    strcpy(path,dirname(temp));
                    strcpy(temp,cookie_path);
                    strcat(path,"/");
                    delete(Btree,path,temp,Blist,*Slist,file);

                } /* delete file from both trees */
                strcpy(previous_event_name,event_name(event));
                strcpy(previous_event_target_name,target_name(event));
                strcpy(previous_event_target_type,target_type(event));
            }
            //advance read_ptr to the beginning of the next event
            read_ptr += EVENT_SIZE + event->len;
        }
        //check to see if a partial event remains at the end
        if( read_ptr < length ) {
            //copy the remaining bytes from the end of the buffer to the beginning of it
            memcpy(buffer, buffer + read_ptr, length - read_ptr);
            //and signal the next read to begin immediatelly after them
            read_offset = length - read_ptr;
        } else
            read_offset = 0;
    }
    close(fd);
}

const char * target_type(struct inotify_event *event) {
    if( event->len == 0 )
        return "";
    else
        return event->mask & IN_ISDIR ? "directory" : "file";
}

const char * target_name(struct inotify_event *event) {
    return event->len > 0 ? event->name : NULL;
}

const char * event_name(struct inotify_event *event) {
    if (event->mask & IN_ACCESS)
        return "access";
    else if (event->mask & IN_ATTRIB)
        return "attrib";
    else if (event->mask & IN_CLOSE_WRITE)
        return "close write";
    else if (event->mask & IN_CLOSE_NOWRITE)
        return "close nowrite";
    else if (event->mask & IN_CREATE)
        return "create";
    else if (event->mask & IN_DELETE)
        return "delete";
    else if (event->mask & IN_DELETE_SELF)
        return "watch target deleted";
    else if (event->mask & IN_MODIFY)
        return "modify";
    else if (event->mask & IN_MOVE_SELF)
        return "watch target moved";
    else if (event->mask & IN_MOVED_FROM)
        return "moved out";
    else if (event->mask & IN_MOVED_TO)
        return "moved into";
    else if (event->mask & IN_OPEN)
        return "open";
    else
        return "unknown event";
}
