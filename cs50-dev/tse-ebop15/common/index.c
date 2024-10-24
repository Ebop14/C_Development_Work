// Written by Ethan Child for CS50
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "index.h"
#include "../libcs50/set.h"
#include "../libcs50/file.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "../libcs50/mem.h"
#include "word.h"

typedef hashtable_t index_t;

index_t* index_new(const int num_slots){
    if(num_slots <= 0){
        return NULL;
    }
    else{
        index_t* index = hashtable_new(num_slots);
        return index;
    }
}

int index_add(index_t* index, char* word, int docID){
    // null checks for index and word
    if(index == NULL || word == NULL){
        return 3;
    }
    // normalize the word (should this be freed later?)
    word = normalizeWord(word);

    // create a counter object
    counters_t* temp = NULL;

    // find the counters object that maps to the word we're using
    if( (temp = hashtable_find(index, word)) == NULL){
        return 2;
    }

    // call counters_add on that counter. 
    counters_add(temp, docID);
    // counters_delete(temp);
    return 0;
}

bool index_set(index_t* index, char* word, int docID, int count){
    return counters_set(hashtable_find(index, word), docID, count);
    // return counters_set((index, word, docID), docID, count);
}

// add indexSave: write from the index to a file with format:
    // Word docID wordCount docID wordcount ...

void countersPrintHelper(void* arg, const int key, const int item){
    FILE* fp = arg;
    fprintf(fp, " %d %d", key, item);
}

void indexPrintHelper(void* arg, const char* key, void* item){
    // iterate through every key (word) in the hashtable
    FILE* fp = arg;
    fprintf(fp, "%s", key);
    counters_iterate(item, fp, countersPrintHelper);
    fprintf(fp, "\n");
}

int index_save(index_t* index, char* file){
    if(index == NULL || file == NULL){
        printf("\nsomething bad is null\n");
        return 2;
    }
    FILE* fp;
    if( (fp = fopen(file, "w")) == NULL){
        printf("\nnot working\n");
        return 1;
    }
    // write from the index
        // &some writer function, writer function
    hashtable_iterate(index, fp, indexPrintHelper);
    fclose(fp);
    return 0;
}

// index_load: read from an index file and return an index.
index_t* index_load(char* file_name){
    // open the file pointer
    FILE* fp;

    // read the file and check if null
    if( (fp = fopen(file_name, "r")) == NULL){
        return NULL;
    }

    // find the number of lines in the index file
        // this is also the number of words
    int numLines = file_numLines(fp);

    // create the index
    index_t* index = index_new(numLines);

    // if the index is NULL
    if(index == NULL){
        printf("\nindex isn't being created\n");
        return NULL;
    }

    char* word;
    while( (word = file_readWord(fp)) != NULL){
        int docID;
        int occurances;

        while( (fscanf(fp, " %d %d", &docID, &occurances)) == 2){
            index_insert(index, word, docID);

            index_set(index, word, docID, occurances);
        }
        free(word);
    }

    fclose(fp);
    return index;
}


bool index_insert(index_t* index, const char* word, int docID){
    if(index == NULL || word == NULL){
        return false;
    }
    else{
        counters_t* counter = counters_new();
        counters_add(counter, docID);
        if(hashtable_insert(index, word, counter)){
            return true;
        }
        else{
            return false;
        }
    }
}


void* index_find(index_t* index, const char* word){
    if(index == NULL || word == NULL){
        return NULL;
    }
    else{
        return hashtable_find(index, word);
    }
}

void item_delete(void* item){
    counters_t* arg = item;
    if(item != NULL){
        counters_delete(arg);
    }
}

void index_delete(index_t* index){
    if(index != NULL){
        hashtable_delete(index, item_delete);
    }
}
