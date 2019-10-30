#include "sync.h"
int main(int argc,char *argv[]){
        tree *Stree=NULL,*Btree=NULL;
        char source[256],backup[256],**paths_of_source_directories,**paths_of_destination_directories;
        i_node *Slist=NULL,*Blist=NULL;
        int i,source_num=0,backup_num=0;
        i=strlen(argv[1]);
        if(argv[1][i-1]!='/') {
                strcpy(source,argv[1]);
                strcat(source,"/");
        }
        else strcpy(source,argv[1]);
        i=strlen(argv[2]);
        if(argv[2][i-1]!='/') {
                strcpy(backup,argv[2]);
                strcat(backup,"/");
        } /* fix source and backup paths so we do not get errors in initialize-insert functions */
        else strcpy(backup,argv[2]);
        initialize(source,&Stree,&Slist);
        initialize(backup,&Btree,&Blist);
        synchronize(&Btree,Stree,&Blist,Slist); /* compare and sychronize source and backup */
        paths_of_source_directories=malloc(256 *sizeof(char *));
        paths_of_destination_directories=malloc(256 *sizeof(char *));
        for(i=0; i<256; i++) {
                paths_of_source_directories[i]=malloc(256*sizeof(char ));
                paths_of_destination_directories[i]=malloc(256*sizeof(char ));
        }
        directories_traverse(Btree,&backup_num,paths_of_destination_directories);
        directories_traverse(Stree,&source_num,paths_of_source_directories);
        function(&Blist,&Slist,&Btree,&Stree,source_num,paths_of_source_directories,paths_of_destination_directories);
}
