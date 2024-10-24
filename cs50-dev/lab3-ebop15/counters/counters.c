#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "counters.h"
#include "../lib/mem.h"

typedef struct countersnode {
    int key;
    int count;
    struct countersnode *next;
} countersnode_t;

typedef struct counters {
    struct countersnode *head;
} counters_t;

static countersnode_t* countersnode_new(const int key){
    // allocate memory to node
    countersnode_t* node = mem_malloc(sizeof(countersnode_t));
    // allocate memory to key
    // copy key value to counter key
    node->count = 0;
    node->key = key;
    return node;
}

counters_t* counters_new(void){
    counters_t* counters = mem_malloc(sizeof(counters_t));
    if(counters == NULL){
        return NULL;
    }
    else{
        counters->head = NULL;
        return counters; 
    }
}

int counters_add(counters_t* ctrs, const int key){
    if(ctrs != NULL && key >= 0){
        // check if the key is in the set.
        for(countersnode_t* node = ctrs->head; node != NULL; node = node->next){
            if(key == node->key){
                printf("key in use");
                node->count += 1;
                return node->count;
            }
        }
        countersnode_t* new = countersnode_new(key);
        new->next = ctrs->head;
        ctrs->head = new;
        new->count = 1;
        return new->count;
    }
    return 0;
}

int counters_get(counters_t* ctrs, const int key){
    if(ctrs != NULL && key >= 0){
        for(countersnode_t* node = ctrs->head; node != NULL; node = node->next){
            if(node->key == key){
                return node->count;
            }
        }
    }
    return 0;
}

bool counters_set(counters_t* ctrs, const int key, const int count){
    if(ctrs != NULL && key >= 0 && count >= 0){
        for(countersnode_t* node = ctrs->head; node != NULL; node = node->next){
            if(node->key == key){
                node->count = count;
                printf("counter set");
                return true;
            }
        }
        printf("\ncounter doesn't exist yet. Creating one now. \n");
        if(counters_add(ctrs, key) != 0){
            return true;
        }
        else{
            return false;
        }
    }
    return false;
}

void counters_print(counters_t* ctrs, FILE* fp){
    if(fp != NULL){
        if(ctrs != NULL){
            fputc('{', fp);
            for(countersnode_t* node = ctrs->head; node != NULL; node = node->next){
                fprintf(fp, "%d=%d", node->key, node->count);
                fputc(',', fp);
            }
            fputc('}', fp);
        }
        else{
            fputs("(null)", fp);
        }
    }
}

void counters_iterate(counters_t* ctrs, void* arg, void (*itemfunc)(void* arg, const int key, const int count)){
    if (ctrs != NULL && arg != NULL && itemfunc != NULL) {
    // call itemfunc with arg, on each item
    for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
        (*itemfunc)(arg, node->key, node->count); 
    }
  }
}

void counters_delete(counters_t* ctrs){
    if(ctrs != NULL){
    for (countersnode_t* node = ctrs->head; node != NULL; ) {
            countersnode_t* next = node->next;
            mem_free(node);
            node = next;
    }
    mem_free(ctrs);
    }
#ifdef MEMTEST
  mem_report(stdout, "End of counters_delete");
#endif 
}