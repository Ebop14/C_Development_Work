#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../libcs50/file.h"
#include "../libcs50/webpage.h"
#include "../libcs50/mem.h"

char* directory_name;
FILE* fp;

bool pagedir_init(const char* pageDirectory){
    char* temp = mem_malloc(strlen(pageDirectory) + strlen("/.crawler") + 1);
    strcpy(temp, pageDirectory);
    strcat(temp, "/.crawler");
    if( (fp = fopen(temp, "w")) == NULL){
        printf("couldn't open pathname");
        return false;
    }
    fclose(fp);
    free(temp);
    return true;
}

void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID){
    FILE* fp;
    if(pagedir_init(pageDirectory)){
        // string_ID
        char* string_ID = mem_malloc(sizeof(docID));
        sprintf(string_ID, "%d", docID);

        // filePath
        char* filePath = mem_malloc(strlen(pageDirectory) + strlen("/") + strlen(string_ID) + 1);
        strcpy(filePath, pageDirectory);
        strcat(filePath, "/");
        strcat(filePath, string_ID);

        // print statements        
        if( (fp = fopen(filePath, "w")) != NULL){
            char* URL = webpage_getURL(page);
            printf("%s", URL);
            fputs(URL, fp);
            fputc('\n', fp);

            int depth = webpage_getDepth(page);
            fprintf(fp, "%d", depth);
            fputc('\n', fp);
            // fprintf(fp, "%d", docID);
            // fputc('\n', fp);
            fputs(webpage_getHTML(page), fp);
            free(URL);
        }
        free(string_ID);
        free(filePath);
        fclose(fp);
    }
}

webpage_t* pagedir_load(char* filePath){
    FILE* fp;
    if( (fp = fopen(filePath, "r")) == NULL){
        return NULL;
    }
    else{
        rewind(fp);
        char* URL = file_readLine(fp);
        char* depth = file_readLine(fp);

        if(URL == NULL || depth == NULL){
            printf("\nnull url or depth\n");
            return NULL;
        }


        webpage_t* page = webpage_new(URL, atoi(depth), NULL);

        
        if(!webpage_fetch(page)){
            return NULL;
        }
        mem_free(depth);
        fclose(fp);
        return page;
    }
}