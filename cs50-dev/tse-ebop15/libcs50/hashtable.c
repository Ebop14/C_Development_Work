#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "hash.h"
#include "set.h"
#include "mem.h"

// local types
typedef struct hashtable {
    set_t** sets;
    int table_slots;
} hashtable_t;

// global types
hashtable_t* hashtable_new(const int num_slots){
    // if there are no slots, or if there are less than 0 slots
    if(num_slots <= 0){
        return NULL;
    }
    else{
        // allocate memory for the table
        hashtable_t* table = calloc(num_slots, sizeof(hashtable_t));
        // remember the number of slots
        table->table_slots = num_slots;
        // allocate the slot memory, in an array of each size set
        table->sets = calloc(num_slots, sizeof(set_t*));
        // loop over every slot, create a new set in the slot
        for(int i = 0; i < num_slots; i++){
            table->sets[i] = set_new();
        }
        return table; 
    }
}

bool hashtable_insert(hashtable_t* ht, const char* key, void* item){
    // null checks
    if(ht == NULL || key == NULL || item == NULL){
        return false;
    }
    else{
        // get the hash from the hashing function
        unsigned long address = hash_jenkins(key, ht->table_slots);
        // check if set_insert works
        if(set_insert(ht->sets[address], key, item)){
            return true;
        }
    }
    return false;
}

void* hashtable_find(hashtable_t* ht, const char* key){
    // if the hashing function or key are null, return null
    if(ht == NULL || key == NULL){
        return NULL;
    }
    else{
        // get the address from the function
        unsigned long address = hash_jenkins(key, ht->table_slots);
        // return the item
        return set_find(ht->sets[address], key);
    }
}


void hashtable_print(hashtable_t* ht, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item)){
    // null checks
    if(fp != NULL){
        if(ht != NULL){
            // iterate through every table slot
            for(int i = 0; i < ht->table_slots; i++){
                // call set print on the set in the table slot already there
                set_print(ht->sets[i], fp, itemprint);
                printf("\n");
            }
        }
        else{
            fputs("(null)", fp);
        }
    }
}

void hashtable_iterate(hashtable_t* ht, void* arg, void (*itemfunc)(void* arg, const char* key, void* item) ){
    // null checks
    if (ht != NULL && itemfunc != NULL) {
        // call itemfunc with arg, on each iteration of a slot
        for (int i = 0; i < ht->table_slots; i++) {
            set_iterate(ht->sets[i], arg, itemfunc);
        }
    }
}

void hashtable_delete(hashtable_t* ht, void (*itemdelete)(void* item) ){
    // null checks
    if(ht != NULL){
        // call set_delete on every slot
        for (int i = 0; i < ht->table_slots; i++) {
            set_delete(ht->sets[i], itemdelete);
        }
        // free everything not freed by set_delete
        free(ht->sets);
        free(ht);
    }
#ifdef MEMTEST
  mem_report(stdout, "End of counters_delete");
#endif
}