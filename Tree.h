#ifndef header_file_tree
#define header_file_tree
#include "inode.h"
typedef enum {file,directory} mode;
typedef char * string;
typedef struct Tree {
        char name[256];
        mode m;
        i_node *inode;
        struct Tree *files;
        struct Tree *directories;
        struct Tree *next;
}tree;
tree *insert(tree *,string,string,mode,i_node **,ino_t,time_t,off_t);
void deleteRec(tree **, i_node **,i_node *,int);
void tree_modify(tree *,char *,ino_t,time_t,off_t,i_node **,i_node *);
tree * delete(tree **,string,string,i_node **,i_node *,mode);
tree *find(tree *,string,string,mode);
void directories_traverse(tree *,int *,char **);
void compare(tree *,tree *,tree **,tree **,string,string,i_node *,i_node *);
void print_traverse(tree *,int);
#endif
