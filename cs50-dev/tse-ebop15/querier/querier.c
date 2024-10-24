// Written by Ethan Child for CS50
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "../libcs50/bag.h"
#include "../common/word.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/file.h"
#include "../libcs50/mem.h"
#include "../common/pagedir.h"
#include "../common/index.h"

counters_t* build_score(char* pageDirectory, char* indexFilename, char* cleanedLine);
counters_t* subgroup_intersection(bag_t* subGroup, index_t* index);
void subgroup_union(counters_t* subgroupCounter,counters_t** toReturn);
char* clean_line(char* inputLine);
void find_max(void* arg, const int key, const int count);
void print_scores(counters_t* scores, char* pageDirectory);
void delete_helper(void* item);
void union_helper(void* arg, const int key, const int count);
int get_max_docID(const char* pageDirectory);
static void itemcount(void* arg, const int key, const int count);

int maxDocID = 0;

int main(const int argc, char* argv[]){

    if(argc != 3){
        fprintf(stderr, "\nwrong number of arguments\n");
        return 1;
    }

    maxDocID = get_max_docID(argv[1]);

    char* line;

    while( ( line = file_readLine(stdin) ) != NULL){
        char* cleanedLine = clean_line(line);

        printf("Query: %s", cleanedLine);
        counters_t* scores = build_score(argv[1], argv[2], cleanedLine);
        mem_free(cleanedLine);
        print_scores(scores, argv[1]);

        counters_delete(scores);
        
        mem_free(line);
        // print the score
    } 
}

counters_t* build_score(char* pageDirectory, char* indexFilename, char* cleanedLine){

     if(pageDirectory == NULL || indexFilename == NULL || cleanedLine == NULL){
        fprintf(stderr, "\nnull parameters\n");
        return NULL;
    }

    // edits the line to end in or, so it includes the last word
    char* endInOr = mem_malloc(strlen(cleanedLine) + strlen(" or") + 1);

    strcpy(endInOr, cleanedLine);

    strcat(endInOr, " or");

    endInOr[strlen(endInOr)] = '\0';
   
    // tests whether the pageDirectory was crawler-generated
    char* crawlerTest = mem_malloc(strlen(pageDirectory) + strlen("/.crawler") + 1);
    strcpy(crawlerTest, pageDirectory);
    strcat(crawlerTest, "/.crawler");

    FILE* fp;
    
    if( (fp = fopen(crawlerTest, "r")) == NULL ){
        fprintf(stderr, "\nerror opening page directory, may not be crawler-produced\n");
        mem_free(endInOr);
        return NULL;
    }

    mem_free(crawlerTest);
    fclose(fp);

    // loads the index from the index's Filename
    index_t* loadedIndex = index_load(indexFilename);

    if(loadedIndex == NULL){
        fprintf(stderr, "\nerror loading index from filename\n");
        index_delete(loadedIndex);
        mem_free(endInOr);
        return NULL;
    }

    // a boolean that says whether an operator was observed before the current word
    bool operatorBefore = true;

    counters_t* toReturn = counters_new();

    // a bag containing the subgroup of words grouped by "or"
    bag_t* subGroup = bag_new();

    int i = 0;

    // loop over every character in the word
    while(i < strlen(endInOr)){
        
        // isolate the individual word
        char* word = mem_malloc(strlen(endInOr) + 1);
        word[0] = '\0';
        // loop over every word
        while(isspace(endInOr[i]) == 0 && i < strlen(endInOr)){
            char* temp = mem_malloc(2);
            temp[0] = endInOr[i];
            temp[1] = '\0';
            strcat(word, temp);
            mem_free(temp);
            i += 1;
        }
        
        // after we reach a space, add one more to i to skip over it
        i += 1;

        // NULL check
        if(word == NULL){
            // error message
            fprintf(stderr, "\nWord is NULL\n");
            // delete the bag
            bag_delete(subGroup, delete_helper);
            // free endInOr
            mem_free(endInOr);
            // delete the index
            index_delete(loadedIndex);
            exit(9);
        }

        // if the word is the "and" operator
        if(strcmp(word, "and") == 0){
            // if the last word was an operator
            if(operatorBefore){
                // erorr message
                fprintf(stderr, "\ncannot have two operators in a row/have and as the first/last word\n");
                // free word
                mem_free(word);
                // delete the bag
                bag_delete(subGroup, delete_helper);
                // free endInOr
                mem_free(endInOr);
                // delete the index
                index_delete(loadedIndex);
                exit(1);
            }
            // set the operator observed to true
            else{
                operatorBefore = true;
            }
        }
        // if the word is the "or" operator
        else if(strcmp(word, "or") == 0){
            // if the last word was an operator
            if(operatorBefore){
                // error message
                fprintf(stderr, "\ncannot have two operators in a row/have or as the first/last word\n");
                // free word
                mem_free(word);
                // delete the bag
                bag_delete(subGroup, delete_helper);
                // free endInOr
                mem_free(endInOr);
                // delete the index
                index_delete(loadedIndex);
                exit(2);
            }
            // if the last word was regular
            else{
                // create a new subGroup counter
                counters_t* subgroupCounter;
                // This should take the minimum of everything in the subgroup
                subgroupCounter = subgroup_intersection(subGroup, loadedIndex);
                // this takes the aggregate score thus far and increments it
                subgroup_union(subgroupCounter, &toReturn);

                counters_delete(subgroupCounter);
                // function here that changes &toReturn and takes in the new one as a parameter
                // iterate it here. Add it to the subgroup. 

                // now we have an empty bag subGroup
                // new subGroup
                operatorBefore = true;
            }
        }
        else{ // it's a regular word
            operatorBefore = false;
            bag_insert(subGroup, word);
        }
    }
    mem_free(endInOr);
    index_delete(loadedIndex);
    bag_delete(subGroup, delete_helper);
    return toReturn;
}

// take the minimum value of a group of words' occurrances in every docID
counters_t* subgroup_intersection(bag_t* subGroup, index_t* index){
    // null checks
    if(subGroup == NULL || index == NULL){
        fprintf(stderr, "\nnull error with the bag or index\n");
        exit(4);
    }
    // bag for second iteration
    bag_t* secondBag = bag_new();

    // extract it from the subgroup
    counters_t* toReturn = counters_new();
    if(toReturn == NULL){
        fprintf(stderr, "\nerror making counters in subgroup\n");
        return NULL;
    }
    bool notFound = false;
    // extract words from the subgroup
    char* word;
    while( (word = bag_extract(subGroup)) != NULL){ // every word in the subgroup
        // of a given word, foundDocs maps (docID, appearances)
        counters_t* foundDocs;
        // one word isn't in the subgroup. all should be equal to zero
        if( (foundDocs = index_find(index, word)) == NULL){
            notFound = true;
        }        
        bag_insert(secondBag, word);
        // iterate over every docID
        int docID = 1;
        // iterate over every docID in the pageDirectory
        while( docID <= maxDocID ){
            if(notFound){
                counters_set(toReturn, docID, 0);
            }
            // if the docID exists in foundDocs
            // if it doesn't exist in the toReturn counters
            else if(counters_get(toReturn, docID) == 0){
                counters_add(toReturn, docID);
                counters_set(toReturn, docID, counters_get(foundDocs, docID));
            }
            // if it does exist
            else{
                if( counters_get(toReturn, docID) > counters_get(foundDocs, docID)){
                    counters_set(toReturn, docID, counters_get(foundDocs, docID));
                }
            }
            docID += 1;
        }

    }


    // second pass-through
    char* secondWord;
    while( (secondWord = bag_extract(secondBag)) != NULL){ // every word in the subgroup
        // of a given word, foundDocs maps (docID, appearances)
        counters_t* foundDocs = index_find(index, secondWord);

        // iterate over every docID
        int docID = 1;

    
        // iterate over every docID in the pageDirectory
        while( docID <= maxDocID ){
                // if it doesn't exist in the toReturn counters
            if( counters_get(toReturn, docID) > counters_get(foundDocs, docID)){
                counters_set(toReturn, docID, counters_get(foundDocs, docID));
            }
            docID += 1;
        }
        // counters_delete(foundDocs);
    }
    bag_delete(secondBag, delete_helper);
    mem_free(secondWord);

    return toReturn;
}

int get_max_docID(const char* pageDirectory){    
    int docID = 1;
    char* str_docID = mem_malloc(6);
    sprintf(str_docID, "%d", docID);
    
    char* testDirectory;
    testDirectory = mem_malloc(strlen(pageDirectory) + strlen(str_docID) + 2);

    strcpy(testDirectory, pageDirectory);
    strcat(testDirectory, "/");
    strcat(testDirectory, str_docID);
    testDirectory[strlen(testDirectory)] = '\0';


    FILE* fp;

    while( (fp = fopen(testDirectory, "r")) != NULL){
        fclose(fp);

        docID += 1;
        // realloc(str_docID, 6);
        sprintf(str_docID, "%d", docID);

        // realloc(testDirectory, strlen(pageDirectory) + strlen(str_docID) + 2);
        strcpy(testDirectory, pageDirectory);
        strcat(testDirectory, "/");
        strcat(testDirectory, str_docID);
        testDirectory[strlen(testDirectory)] = '\0';

    } 
    mem_free(str_docID);
    mem_free(testDirectory);
    return docID - 1;
}


void subgroup_union(counters_t* subgroupCounter,counters_t** toReturn){
    // we now add the subgroupCounter to the toReturn counters data structure
    if(subgroupCounter == NULL || toReturn == NULL){
        fprintf(stderr, "\nnull parameters\n");
        return;
    }
    int docID = 1;
    while(docID <= maxDocID){
        int oldScore = counters_get(*toReturn, docID);
        int toAdd = counters_get(subgroupCounter, docID);
        if(toAdd != 0){
            counters_set(*toReturn, docID, oldScore + toAdd);
        }
        docID += 1;
    }
}

char* clean_line(char* inputLine){
    char* cleanedLine = mem_malloc(strlen(inputLine) + 1);

    bool previous_space = false;

    int extra_spaces = 0;

    int i = 0;
    while(i < strlen(inputLine)){
        if(isalpha(inputLine[i]) != 0){
            cleanedLine[i - extra_spaces] = tolower(inputLine[i]);
            previous_space = false;
        }
        else if(isspace(inputLine[i]) != 0){
            if(!previous_space){ // if the last character wasn't a space
                cleanedLine[i - extra_spaces] = inputLine[i];
                
                previous_space = true;
            }
            else{ // if previous_space
                extra_spaces += 1;
            }
        }
        else if(isalpha(inputLine[i]) == 0){
            fprintf(stderr, "\nnon-alphabetical character\n");
            mem_free(cleanedLine);
            exit(1);
        }
        cleanedLine[i + 1] = '\0';
        i += 1;
    }
    return cleanedLine;
}

void find_max(void* arg, const int key, const int count){
    int* temp_highest = (int*) arg;
    if(count > temp_highest[1]){
        (temp_highest[0]) = key;
        (temp_highest[1]) = count;
    }
}

void print_scores(counters_t* scores, char* pageDirectory){
    if(scores == NULL){
        exit(1);
    }
    // set the rank
    int rank = 1;

    int highest_numbers[2] = {0, 0};

    int total_docs = 0;

    counters_iterate(scores, &highest_numbers, find_max);

    counters_iterate(scores, &total_docs, itemcount);

    printf("\nMatching %d documents:\n", total_docs);

    // loop through the docIDs
    int highest;
    while( (highest = counters_get(scores, highest_numbers[0]) != 0) ){
        // path to get the URL of the current highest score
        char* str_docID = mem_malloc(6);
        sprintf(str_docID, "%d", highest_numbers[0]);
        char* path = mem_malloc(strlen(pageDirectory) + strlen(str_docID) + strlen("/") + 1);
        
        // build the pageDirectory path
        strcpy(path, pageDirectory);
        strcat(path, "/");
        strcat(path, str_docID);

        // new file pointer that points to the highest docID
        FILE* fpHighest;
        // if we can't open the file with the highest docID
        if( (fpHighest = fopen(path, "r")) == NULL){
            mem_free(path);
            fprintf(stderr, "\nerror opening file\n");
            exit(1);
        }
        // the URL
        char* URL;
        if( (URL = file_readLine(fpHighest)) != NULL){
            // if we iterate through every document counter and non are greater than 0
            if(highest_numbers[1] == 0){
                break;
            }
            // print the score
            fprintf(stderr, "score %d doc %d: %s\n", highest_numbers[1], highest_numbers[0], URL);
            // set the highest to 0
            counters_set(scores, highest_numbers[0], 0);

            // free the URL
            mem_free(URL);
        }
        else{
            fprintf(stderr, "\nerror getting URL\n");
        }

        // close the highest document ID
        mem_free(str_docID);
        fclose(fpHighest);



        // increment the rank
        rank += 1;
        mem_free(path);
        counters_set(scores, highest_numbers[0], 0);
        highest_numbers[0] = 0;
        highest_numbers [1] = 0;
        counters_iterate(scores, &highest_numbers, find_max);
        
    }
    if(rank == 1){
        printf("\nNo matches\n");
    }
}

void delete_helper(void* item)
{
  if (item != NULL) {
    mem_free(item);   
  }
}

static void itemcount(void* arg, const int key, const int count)
{
  int* nitems = arg;

  if (nitems != NULL && key >= 0){
    (*nitems)++;
  }
}