// Written by Ethan Child for CS50
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../libcs50/bag.h"
#include "../common/index.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/webpage.h"
#include "../libcs50/file.h"
#include "../libcs50/mem.h"
#include "../libcs50/counters.h"
#include "../common/pagedir.h"
#include "../common/word.h"


int index_page(index_t* index, webpage_t* page, const int docID);

int index_build(const char* pageDirectory, char* writeTo);

int
main(const int argc, char *argv[]){
    FILE* fp;
    if(argc != 3){
        return 1;
    }

    char* crawlerCheck = mem_malloc(strlen(argv[1]) + strlen("/.crawler") + 1);
    strcpy(crawlerCheck, argv[1]);
    strcat(crawlerCheck, "/.crawler");

    if( (fp = fopen(crawlerCheck, "r")) == NULL){
        mem_free(crawlerCheck);
        printf("\npageDirectory does not have a /.crawler file, and was thus not written by crawler\n");
        return 2;
    }
    fclose(fp);
    index_build(argv[1], argv[2]);
    mem_free(crawlerCheck);
    return 0;
}


int index_page(index_t* index, webpage_t* page, const int docID){
    // start at pos 0
    int pos = 0;
    char* result;
    int i = 0;
    // go through word by word
    while( (result = webpage_getNextWord(page, &pos)) != NULL){
        // if the word is larger than three letters
        if(strlen(result) >= 3){

            // normalize the word
            // char* normal_result = mem_malloc(strlen(result) + 1);
            result = normalizeWord(result);

            // count the number of repeats
            int* repeats;

            // turn the docID into a string
            char* docID_str = mem_malloc(80);
            sprintf(docID_str, "%d", docID);
            
            // search the index for the word
            if( (repeats = index_find(index, result)) == NULL){
                // if it isn't in the index

                // create a temporary counter to hold our information
                counters_t* temp_counter = counters_new();

                // add the docID to the counter, initializing it to 0
                counters_add(temp_counter, docID);

                // insert said temporary counter into the index
                if(!index_insert(index, result, docID)){
                    counters_delete(temp_counter);
                    mem_free(result);
                    return 1;
                }

                // delete the temporary counter
                counters_delete(temp_counter);
            }
            // the word DOES exist in our counter
            else{
                if(index_add(index, result, docID) != 0){
                    mem_free(result);
                    return 2;
                }
            } 
            mem_free(docID_str);
            // mem_free(result); 
        }
        mem_free(result);
        i += 1;
    }
    return 0;
}


int index_build(const char* pageDirectory, char* writeTo){
    index_t* index = index_new(200);
    int docID = 1;
    // not really sure how I can differentiate between when it's a bad file and
    // when the docIDs are done 
    while(true){
        // allocate the docID string
        char* str_docID = mem_malloc(6);
        sprintf(str_docID, "%d", docID);

        // temp is the path to the file we want to parse
        char* temp = mem_malloc(strlen(pageDirectory) + strlen(str_docID) + 2);
        strcpy(temp, pageDirectory);
        strcat(temp, "/");
        strcat(temp, str_docID);
        
        // loads a webpage from the document file page
        webpage_t* page = NULL;

        // it's a problem with how page is loading
        if( (page = pagedir_load(temp)) != NULL){
            // if the page directory loads, call indexPage
            index_page(index, page, docID);
        }
        else{
            mem_free(temp);
            mem_free(str_docID);
            break;
        }
        docID += 1;
        mem_free(str_docID);
        mem_free(temp);
        webpage_delete(page);
    }
    index_save(index, writeTo);
    index_delete(index);
    return 0;
}


