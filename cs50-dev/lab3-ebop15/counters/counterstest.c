/* 
 * bagtest.c - test program for CS50 bag module
 *
 * usage: read names from stdin, each on one line
 *
 * This program is a "unit test" for the bag module.
 * It does a decent job of testing the module, but is
 * not a complete test; it should test more corner cases.
 *
 * CS50, April 2019, 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "counters.h"
#include "../lib/file.h"

static void itemcount(void* arg, const int key, const int count);

/* **************************************** */
int main() 
{
  counters_t* ctr1 = NULL;           // one counter
  char* placeholder = NULL;     // a placeholder for strings to cast to int
  int key1 = 0;            // a key in the counter
  int count1 = 0;           // a count in the counter
  int keycount = 0;            // number of keys put in the counter

  printf("Create first counter...\n");
  ctr1 = counters_new();
  if (ctr1 == NULL) {
    fprintf(stderr, "counter_new failed for ctr1\n");
    return 1;
  }

  // printf("\nTest insert with NULL ctrs, NULL key...\n");
  // counters_add(ctr1, NULL);
  // printf("\nTest insert with good ctrs, NULL key...\n");
  // counters_add(ctr1, NULL);
  // printf("\nTest insert with NULL ctrs, good key...\n");
  // counters_add(NULL, 1);

  printf("\nNumber of keys (should be zero): ");
  keycount = 0;

  printf("\nTesting counters_add with unique keys...\n");
  // read lines from stdin
  keycount = 0;
  while (!feof(stdin)) {
    if( (placeholder = file_readLine(stdin)) != NULL){
      key1 = atoi(placeholder);
      counters_add(ctr1, key1);
      keycount++;
    }
    free(placeholder);
  }

  printf("\nCount (should be %d): ", keycount);

  printf("\nThe counter:\n");
  counters_print(ctr1, stdout);
  printf("\n");

  printf("\nAdding a duplicate to the counter\n");
  counters_add(ctr1, 1);
  counters_add(ctr1, 1);
  counters_add(ctr1, 1);
  counters_add(ctr1, 2);

  printf("\nThe new counter:\n");
  counters_print(ctr1, stdout);
  printf("\nTesting NULL counter print\n");
  counters_print(NULL, stdout);
  printf("\nTesting NULL file print\n");
  counters_print(ctr1, NULL);

  printf("\nTesting counters_get\n");
  printf("\nThe count for key 1: %d, should be 4 \n", counters_get(ctr1, 1));
  printf("\nThe count for key 2: %d, should be 2\n", counters_get(ctr1, 2));
  printf("\nThe count for key 2: %d, should be 1", counters_get(ctr1, 2));
  printf("\nTesting bad counter\n");
  printf("%d", counters_get(NULL, 1));

  printf("\nTesting counters_set\n");
  if(counters_set(ctr1, 1, 50)){
    printf("\nSuccess\n");
    counters_print(ctr1, stdout);
  }
  else{
    printf("\nNot working\n");
  }

  printf("\nTesting counters_set with NULL counter\n");
  if(counters_set(NULL, 1, 50)){
    printf("\nNot working\n");
  }
  else{
    printf("\nSuccess\n");
  }
  
  printf("\nTesting counters_set with bad count\n");
  if(counters_set(ctr1, -1, 5)){
    printf("\nNot working\n");
  }
  else{
    printf("\nSuccess\n");
  }

  printf("\nTesting counters_set with bad count\n");
  if(counters_set(ctr1, 1, -5)){
    printf("\nNot working\n");
  }
  else{
    printf("\nSuccess\n");
  }

  printf("\nTesting counters_set with nonexistent key\n");
  if(counters_set(ctr1, 17, 6)){
    counters_print(ctr1, stdout);
    keycount++;
    printf("\nThe current keycount: %d\n", keycount);
  }
  else{
    printf("\nNot working\n");
  }


  printf("\nTest counter_iterate, NULL counter\n");
  count1 = 0;
  counters_iterate(ctr1, &count1, itemcount);

  printf("\nCount should be, %d, the count is: %d\n", keycount, count1);
  printf("\ndelete the counter...\n");
  counters_delete(ctr1);
  return 0;
}


/* count the non-null keys in the counter.
 * note here we don't care what kind of item is in counter.
 */
static void itemcount(void* arg, const int key, const int count)
{
  int* nitems = arg;

  if (nitems != NULL && key >= 0){
    (*nitems)++;
  }
}