#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/index.h"
#include "../libcs50/file.h"

int main(const int argc, char* argv[]){
    if(argc != 3){
        return 1;
    }
    // char* oldIndexFileName = argv[1];
    // char* newIndexFileName = argv[2];

    printf("\n1: %s\n", argv[1]);
    printf("\n2: %s\n", argv[2]);
    index_t* index;
    index = index_load(argv[1]);
    if(index == NULL){
        printf("\n index_new failed\n");
        return 2;
    }

    if(index_save(index, argv[2]) != 0){
        printf("\nindex_save failed\n");
        return 4;
    }

    index_delete(index);
    
    return 0;

}