#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

int
readStdin(void){
    bool previous = false;
    char c;
    while((c = getchar()) != EOF){
        if(isalpha(c) == 0){
            if(!previous){
                printf("\n");
                previous = true;
            }
        }
        else{
            previous = false;
            putchar(c);
        }
    }
    return 0;
}

int
readFile(const char *file_name){
    FILE* fp;
    if((fp = fopen(file_name, "r")) == NULL){
        printf("could not open file! \n");
        return 1;
    }
    else{
    bool previous = false;
    char c;
    while((c = fgetc(fp)) != EOF){
        if(isalpha(c) == 0){
            if(!previous){
                printf("\n");
                previous = true;
            }
        }
        else{
            previous = false;
            putchar(c);
        }
    }
        fclose(fp);
        return 0;
    }

}

int 
main(const int argc, const char *argv[]) {
    // boolean flag for space
    // no arguments
    int exit_code = 0;
    if(argc == 1){
        exit_code = readStdin();
    }
    else{
        int i = 1;
        while(i < argc && exit_code == 0){
            if(strcmp(argv[i], "-") == 0){
                exit_code = readStdin();
            }
            else{
                exit_code = readFile(argv[i]);
            }
            i += 1;
        }
    }
    return exit_code;
}