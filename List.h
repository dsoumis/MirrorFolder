#ifndef PROJECT4_LIST_H
#define PROJECT4_LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct node {
    char name[256];
    struct node *next;
};
struct node *push(struct node *,const char*);
struct node *rm_list(struct node *,char *);
#endif
