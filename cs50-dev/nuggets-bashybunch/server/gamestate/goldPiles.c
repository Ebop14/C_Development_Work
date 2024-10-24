// written by Ethan Child for CS50

#include "goldPiles.h"
#include <stdlib.h>
#include "../../libcs50/hashtable.h"
#include "../../libcs50/counters.h"
#include "../common/server_structs.h"
#include "../common/server_structs.c"

typedef hashtable_t goldPiles_t;

goldPiles_t* piles_new(const int num_slots){
    if(num_slots <= 0){
        return NULL;
    }
    else{
        goldPiles_t* piles = hashtable_new(num_slots);
        return piles;
    }
}

bool piles_set(goldPiles_t* piles, char* x, int y, int gold){
    return counters_set(hashtable_find(piles, x), y, gold);
}


void* piles_find(goldPiles_t* pile, const char* x){
    if (!pile || !x) {
        return NULL;
    }
    else {
        return hashtable_find(pile, x);
    }
}

void pile_item_delete(void* item){
    counters_t* arg = item;
    if(item != NULL){
        counters_delete(arg);
    }
}

void piles_delete(goldPiles_t* pile){
    if(pile != NULL){
        hashtable_delete(pile, pile_item_delete);
    }
}


bool piles_add(goldPiles_t* pile, const char* x, const int y, int amount) {
    // Find the counters with key x
    counters_t* counters = hashtable_find(pile, x);

    // If it doesn't exist, create it and insert it into the hashtable
    if (!counters) {
        counters = counters_new();
        if (!counters|| !hashtable_insert(pile, x, counters)) {
            return false;
        }
    }

    // Set the counter for y
    return counters_set(counters, y, amount);
}


void piles_iterate(goldPiles_t* piles, void* arg, void (*itemfunc)(void* arg, const char* key, void* item)){
    // hashtable_iterate(piles, arg, void (counters_iterate)((counters_t*) item, arg, itemfunc));
    hashtable_iterate(piles, arg, itemfunc);
}
