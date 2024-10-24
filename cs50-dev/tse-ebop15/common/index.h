// Written by Ethan Child for CS50

#ifndef __INDEX_H_
#define __INDEX_H_

#include <stdio.h>
#include <stdbool.h>
#include "../libcs50/counters.h"
#include "../libcs50/hashtable.h"

typedef hashtable_t index_t;

index_t* index_new(const int num_slots);

int index_add(index_t* index, char* word, int docID);

int index_save(index_t* index, char* file);

index_t* index_load(char* file_name);

bool index_insert(index_t* index, const char* word, int docID);

void* index_find(index_t* index, const char* word);

void index_delete(index_t* index);

void item_delete(void* item);

bool index_set(index_t* index, char* word, int docID, int count);


#endif // __INDEX_H
