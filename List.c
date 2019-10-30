#include "List.h"
struct node *push(struct node *list,const char *name){ /*push the item at the the start of the list */
        struct node *node;
        if((node=malloc(sizeof(struct node)))==NULL) {
                perror("Error in allocating dynamic memory\n");
                exit(1);
        }
        strcpy(node->name,name);
        node->next=list;
        return node;
}
struct node *rm_list(struct node * list,char *name) {
        struct node *l_temp,*head=list;
        if(!strcmp(list->name,name)) {
                l_temp=list;
                list=list->next;
                free(l_temp);
                return list;
        } /* return new head as the first name must be removed */
        while(strcmp(list->next->name,name)) list=list->next; /* traverse the list until you find the name */
        l_temp=list->next->next;
        free(list->next);
        list->next=l_temp;
        return head;
}
