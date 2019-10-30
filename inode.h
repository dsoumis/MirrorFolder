#ifndef PROJECT4_INODE_H
#define PROJECT4_INODE_H
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include "List.h"
typedef struct Inode{
    ino_t inode_id;
    time_t date_last_modified;
    double size_of_file;
    int counter_of_names;
    struct node *list_of_names;
    struct Inode *destination;
    struct Inode *next;
}i_node;
/*//////////////////////////////////////////////////////////////*/
i_node *create_inode(ino_t,time_t,off_t,char *);
i_node *list_insert(i_node *,i_node *);
i_node *check_list(i_node *,ino_t,char *);
i_node *rm_inode_list(i_node *,ino_t,char *,i_node *);
void free_inode_list(i_node **);
#endif
