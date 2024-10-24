#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "set.h"
#include "mem.h"

// Written by Ethan Child for CS50
typedef struct setnode {
    void* item;
    void* key;
    struct setnode *next;
} setnode_t;

typedef struct set {
    struct setnode *head;
} set_t;

static setnode_t* setnode_new(const char* key, void* item){
    // allocate memory to the set node
    setnode_t* node = mem_malloc(sizeof(setnode_t));
    // allocate memory to the key
    node->key = mem_malloc(sizeof(key) + 1);
    // copy key and item to the space allocated
    strcpy(node->key, key);
    node->item = item;
    // make sure that the key points to the item
    return node;
}

set_t*
set_new(void){
    set_t* set = mem_malloc(sizeof(set_t));
    // null check
    if(set == NULL){
        return NULL;
    }
    else{
        set->head = NULL;
        return set; 
    }
}

bool
set_insert(set_t* set, const char* key, void* item){
    if(set != NULL && item != NULL && key != NULL){
        // check if the key is in the set.
        setnode_t* new = setnode_new(key, item);
        for(setnode_t* node = set->head; node != NULL; node = node->next){
            if(strcmp(node->key, key) == 0){
                printf("key in use");
                free(new->key);
                free(new);
                return false;
            }
        }
        new->next = set->head;
        set->head = new;
        return true;
    }
    return false; 
}

void* set_find(set_t* set, const char* key){
    // null checks
    if(set != NULL && key != NULL){
        for(setnode_t* node = set->head; node != NULL; node = node->next){
            if(strcmp(node->key, key) == 0){
                return node->item;
            }
        }
        return NULL;
    }
    return NULL;
}

void set_print(set_t* set, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item) ){
    if(fp != NULL){
        if(set != NULL){
            fputc('{', fp);
            for(setnode_t* node = set->head; node != NULL; node = node->next){
                if(itemprint != NULL){
                    (*itemprint)(fp, node->key, node->item);
                    fputc(',', fp);
                }
            }
            fputc('}', fp);
        }
        else{
            fputs("(null)", fp);
        }
    }
}

void set_iterate(set_t* set, void* arg, void (*itemfunc)(void* arg, const char* key, void* item) ){
    if (set != NULL && arg != NULL && itemfunc != NULL) {
    // call itemfunc with arg, on each item
    for (setnode_t* node = set->head; node != NULL; node = node->next) {
      (*itemfunc)(arg, node->key, node->item); 
    }
  }
}

void set_delete(set_t* set, void (*itemdelete)(void* item) ){
    if(set != NULL){
        for (setnode_t* node = set->head; node != NULL; ) {
            if(itemdelete != NULL){
                (*itemdelete)(node->item);
            }
            free(node->key);
            setnode_t* next = node->next;
            mem_free(node);
            node = next;
    }
    mem_free(set);
    }
#ifdef MEMTEST
  mem_report(stdout, "End of set_delete");
#endif 
}