// written by Ethan Child for CS50
#ifndef GOLDPILES_H
#define GOLDPILES_H

#include <stdio.h>
#include <stdlib.h>
#include "../../libcs50/hashtable.h"
#include "../../libcs50/counters.h"


typedef hashtable_t goldPiles_t;

goldPiles_t* piles_new(const int num_slots);

bool piles_set(goldPiles_t* piles, char* x, int y, int gold);

void* piles_find(goldPiles_t* pile, const char* x);

bool piles_add(goldPiles_t* pile, const char* x, const int y, int amount);

void piles_iterate(goldPiles_t* piles, void* arg, void (*itemfunc)(void* arg, const char* key, void* item));

void piles_delete(goldPiles_t* index);

#endif
