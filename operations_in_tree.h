#ifndef PROJECT4_OPERATIONS_IN_TREE_H
#define PROJECT4_OPERATIONS_IN_TREE_H
#include "Tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
tree *a(char *,char *,tree **,i_node **,i_node *);
void recursion_for_b(char *);
void b(char *,char *,tree **,i_node **,i_node *);
tree *c(char *,char *,i_node *,tree **,i_node **,i_node *);
void d(char *,char *,tree **,i_node **,i_node *);
void e(char *,char *,i_node *,i_node **,tree **,i_node **,i_node *);
#endif //PROJECT4_OPERATIONS_IN_TREE_H
