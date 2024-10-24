#include "word.h"
#include "../libcs50/mem.h"

char* normalizeWord(char* word){
    char* temp = mem_malloc(strlen(word) + 1);
    if(word == NULL){
        return NULL;
    }
    int i = 0;
    if(word == NULL){
        free(temp);
        return NULL;
    }
    while(i < strlen(word)){
        temp[i] = tolower(word[i]);
        i += 1;
    }
    temp[i] = '\0';
    strcpy(word, temp);
    free(temp);
    return word;
}