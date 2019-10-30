#include "inode.h"
#include "List.h"
i_node *create_inode(ino_t ino,time_t mtime,off_t size,char *name){ /* create new inode */
        i_node *inode;
        if((inode=malloc(sizeof(i_node)))==NULL) {
                perror("Error in allocating dynamic memory\n");
                exit(1);
        }
        inode->inode_id=ino;
        inode->date_last_modified=mtime;
        inode->size_of_file=size;
        inode->counter_of_names=1;
        inode->next=NULL;
        inode->destination=NULL;
        inode->list_of_names=push(NULL,name);
        return inode;
}
i_node *list_insert(i_node *list,i_node *inode){ /* push the inode at the star of the list */
        inode->next=list;
        return inode;
}
i_node *check_list(i_node *list,ino_t inode_id,char *name){
        while(list!=NULL) {
                if(list->inode_id==inode_id) { /* found the inode return its pointer and push the link name */
                        list->list_of_names=push(list->list_of_names,name);
                        (list->counter_of_names)++;
                        return list;
                }
                else list=list->next;
        }
        return NULL; /* didnt find the inode */
}
i_node *rm_inode_list(i_node *list,ino_t inode_id,char *name,i_node *Slist){
        i_node *l_temp,*head=list;
        if(list->inode_id==inode_id) {
                if (--(list->counter_of_names)==0) { /* last link so remove inode */

                        while(Slist!=NULL) {
                                if(Slist->destination!=NULL) {
                                        if(Slist->destination->inode_id==list->inode_id) {
                                                Slist->destination=NULL;
                                                break;
                                        }
                                        Slist=Slist->next;
                                }
                                else Slist=Slist->next;
                        }
                        free(list->list_of_names);
                        l_temp=list;
                        list=list->next;
                        free(l_temp);
                        return list;
                }
        }
        else {
                //printf("|||%ld\n",inode_id);
                while(list->next->inode_id!=inode_id) list=list->next; /* traverse the list until you find the inode */
                if (--(list->next->counter_of_names)==0) { /* last link so remove inode */
                        while(Slist!=NULL) {
                                if(Slist->destination!=NULL) {
                                        if(Slist->destination->inode_id==list->next->inode_id) {
                                                Slist->destination=NULL;
                                                break;
                                        }
                                        Slist=Slist->next;
                                }
                                else Slist=Slist->next;
                        }
                        free(list->next->list_of_names);
                        l_temp=list->next->next;
                        free(list->next);
                        list->next=l_temp;
                        return head;
                }
                list=list->next;
        }
        list->list_of_names=rm_list(list->list_of_names,name); /* remove the name from list of names */
        return head;

}
