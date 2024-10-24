#include <stdio.h>
#include <string.h>
#include "../libcs50/bag.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/webpage.h"
#include "../libcs50/file.h"
#include "../libcs50/mem.h"
#include "../common/pagedir.h"
#include <unistd.h>



static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen){
    int pointer = 0;
    char* URL;
    // while there is another URL in the page
    while( (URL = webpage_getNextURL(page, &pointer)) != NULL){
        char* normal_URL = normalizeURL(URL);
        // if the URL is internal
        if(isInternalURL(normal_URL)){
            // insert webpage into hashtable
            if(hashtable_insert(pagesSeen, normal_URL, "")){
                // if that worked
                // create a webpage_T for the new webpage
                webpage_t* temp_page = webpage_new(normal_URL, webpage_getDepth(page) + 1, NULL);
                //insert it into the bag
                bag_insert(pagesToCrawl, temp_page);
            }
            else{
                free(normal_URL);
            }
        }
        else{
            free(normal_URL);
        }
        free(URL);
    }
}

// changed to static int
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth){
    if(argc != 4){
        printf("\nmore than three arguments!\n");
        exit(1);
    }
    
    *seedURL = argv[1];

    char* normal_URL = normalizeURL(*seedURL);

    *pageDirectory = argv[2];
    
    *maxDepth = atoi(argv[3]);

    if(!isInternalURL(normal_URL)){
        free(normal_URL);
        exit(3);
    }

    free(normal_URL);

    if(!pagedir_init(*pageDirectory)){
        exit(4);
    }
    
    if(*maxDepth < 0 || *maxDepth > 10){
        exit(5);
    }
    return;
}


static void crawl(char* seedURL, char* pageDirectory, const int maxDepth){

    int ID = 0;

    // initialize the hashtable 
    hashtable_t* pagesSeen = NULL;
    pagesSeen = hashtable_new(200);
    hashtable_insert(pagesSeen, seedURL, "");
    if(pagesSeen == NULL){
        hashtable_delete(pagesSeen, NULL);
        return;
    }
    // // add the seed url
    // if(!hashtable_insert(pagesSeen, seedURL, "")){
    //     hashtable_delete(pagesSeen, NULL);
    //     return;
    // }

    // initialize the bag
    bag_t* page_bag = NULL;
    page_bag = bag_new();

    char* seedURL_copy = mem_malloc(strlen(seedURL) + 1);
    strcpy(seedURL_copy, seedURL);
    // create webpage represnting seedURL at depth 0
    webpage_t* current_page;
    if( (current_page = webpage_new(seedURL_copy, 0, NULL)) == NULL ){
        hashtable_delete(pagesSeen, NULL);
        webpage_delete(current_page);
        return;
    }
    


    // add the webpage at depth 0 into the bag
    bag_insert(page_bag, current_page);
    // while bag is not empty
        // note that current_page is a pointer to a bag
    
    while( (current_page = bag_extract(page_bag)) != NULL){
        if(webpage_fetch(current_page)){
            // get the HTML
            // char* HTML = webpage_getHTML(current_page);
            // if webpage not at maxDepth
            pagedir_save(current_page, pageDirectory, ID++); 
            if(webpage_getDepth(current_page) <= maxDepth){
                pageScan(current_page, page_bag, pagesSeen);
            }
        }
        else{
            exit(1);
        }
        webpage_delete(current_page);
    }

    hashtable_delete(pagesSeen, NULL);
    bag_delete(page_bag, NULL);
}

// if passed correctly, the arguments should go, in order:
// crawler 0, seedURL 1, pageDirectory 2, maxDepth 3
// pageDirectory stores the webpages btw
int
main(const int argc, char *argv[]){
    if(argc != 4){
        return 1;
    }

    char* seedURL = NULL;
    char* pageDirectory = NULL;
    int maxDepth = 0;

    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);

    crawl(seedURL, pageDirectory, maxDepth);
    return 0;
}

