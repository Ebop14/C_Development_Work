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
#include "hashtable.h"
#include "../lib/file.h"

static void nameprint(FILE* fp, const char* key, void* item);
static void namedelete(void* item);
static void itemcount(void* arg, const char* key, void* item);

/* **************************************** */
int main() 
{
  hashtable_t* ht1 = NULL;           // one set
  char* name = NULL;            // a name in the set
  char* key = NULL;             // a key in the set
  int num_slots = 5;
  int namecount1 = 0;            // number of names put in the set
  int itemcount1 = 0;             // number of sets in the ht

  printf("Create first ht with 5 slots...\n");
  ht1 = hashtable_new(num_slots);
  if (ht1 == NULL) {
    fprintf(stderr, "hashtable_new failed for ht1\n");
    return 1;
  }

  printf("\nTest with null ht, good key, good item...\n");
  hashtable_insert(NULL, "2025", "Dartmouth");
  printf("test with good ht, good key, null item...\n");
  hashtable_insert(ht1, "2025", NULL); 
  printf("test with good ht, null key, good item...\n");
  hashtable_insert(ht1, NULL, "Dartmouth"); 
  printf("test with null ht, null key, null item...\n");
  hashtable_insert(NULL, NULL, NULL);

  printf("\nTotal item count (should be zero): ");
  itemcount1 = 0;
  hashtable_iterate(ht1, &itemcount1, itemcount);
  printf("%d\n", itemcount1);

  printf("\nTesting ht_insert...\n");
  // read lines from stdin
  namecount1 = 0;
  while (!feof(stdin)) {
    name = file_readLine(stdin);
    key = file_readLine(stdin);
    if (name != NULL && key != NULL) {
      hashtable_insert(ht1, key, name);
      namecount1++;
    }
    free(key);
  }

printf("\nTesting ht_find...\n");
printf("\nGood ht, bad key\n");
hashtable_find(ht1, "noKey");
printf("\nNull ht, good key\n");
hashtable_find(NULL, "Ethan");
printf("\nGood ht, Null key");
hashtable_find(ht1, NULL);
printf("\nGood ht, good key");


char* key_test = hashtable_find(ht1, "key1");
if(strcmp(key_test, "Ethan") == 0){
  printf("working");
  printf("\nkey1 maps to Ethan");
}
else{
  printf("\n The key you have tested is: \"%s\"\n", key_test);
}

  printf("\nTesting duplicate key insert\n");
  hashtable_insert(ht1, "key1", "duplicate");
  printf("\nCount (should be %d): ", namecount1);
  itemcount1 = 0;
  hashtable_iterate(ht1, &itemcount1, itemcount);
  printf("%d\n", itemcount1);

  printf("\nThe ht:\n");
  hashtable_print(ht1, stdout, nameprint);
  printf("\n");


  printf("\nThe ht:\n");
  printf("Count (should be %d): ", itemcount1);
  itemcount1 = 0;
  hashtable_iterate(ht1, &itemcount1, itemcount);
  printf("%d\n", itemcount1);  
  hashtable_print(ht1, stdout, nameprint);
  printf("\n");

  printf("\ndelete the ht...\n");
  hashtable_delete(ht1, namedelete);

  return 0;
}


/* count the non-null items in the set.
 * note here we don't care what kind of item is in set.
 */
static void itemcount(void* arg, const char* key, void* item)
{
  int* nitems = arg;

  if (nitems != NULL && key != NULL && item != NULL)
    (*nitems)++;
}

// print a name, in quotes.
void nameprint(FILE* fp, const char* key, void* item)
{
  char* name = item; 
  if (key == NULL || name == NULL) {
    fprintf(fp, "(null)");
  }
  else {
    fprintf(fp, "\"%s\" ==> \"%s\"", key, name); 
  }
}

// delete a name 
void namedelete(void* item)
{
  if (item != NULL) {
    free(item);   
  }
}