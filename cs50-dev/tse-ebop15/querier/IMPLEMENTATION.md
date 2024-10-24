Parts to the Implementation Spec:
    data structures
    control flow
    detailed prototype functions and parameters 
    error handling and recovery
    testing plan

Data Structures:
We use a few data structures here:
* Bag
* Counters
* Index

Control Flow:
The Querier is implemented in one file, querier.c, with a few functions:
main: 
* validates parameters
* parses the input line
* calls clean_line
* calls build_score on the PageDirectory, the indexFilename, and the cleanedLine, storing the scores in a counters object
* calls print_scores on the scores counters object and the pageDirectory
* continues this loop until EOF

build_score:
builds the score from the pageDirectory, the indexFilename, and the cleanedLine
Pseudocode:
check if the parameters are NULL

check if the pagedirectory is crawler-produced
load the index from the indexFilename

initialize boolean operatorBefore to True to check if the previous word was an operator

create a bag Subgroup to hold the words in a given sentence
create a counters objecet toReturn
for every word in the sentence:
    if the word is "and":
        check if the last word was an operator
        if not, set operatorBefore to True
    if the word is "or":
        check if the last word was an operator
        if not, call subgroup_intersection on a new counter
        call subgroup_union on subgroupCounter and &toReturn
        delete subgroupCounter
        set operatorbefore to true
    if it's a regular word
        add it to the subgroup bag
        set operatorbefore to false

free all relevant memory
return toReturn

subgroup_intersection
    validate parameters
    create a second bag

    create a toReturn counters

    for every word from the subgroup (parameter) bag
        put the word in the second bag
        if the value of our current word is less than our value in toReturn, minimize toReturn
    
    for every word in the second bag
        minimize toReturn again

getMaxDocID
    iterate through every file in pagedirectory
    iterate docID until it can be opened

subgroup_union
    for every docID
        add to the total score of the docID in toReturn
    
clean_line
    for every character in the sentence
        if the character is a space, skip it
        if it's an alphabetical, add it to the clean line
        if it's something else, return an error

print_scores
    initialze a tuple highest_numbers that records the DocID and count of the highest numbers

    iterate through our scores counter and find the max score, store it in highest_numbers

    count the total number of items

    while the biggest score is greater than 0:
        validate our pagedirectory
        get the URL from our docID
        
        print our score, the docID, and the URL

        set the score to 0

        calculate the new highest score
    
Function Prototypes:
counters_t* build_score(char* pageDirectory, char* indexFilename, char* cleanedLine);
counters_t* subgroup_intersection(bag_t* subGroup, index_t* index);
void subgroup_union(counters_t* subgroupCounter,counters_t** toReturn);
char* clean_line(char* inputLine);
void findMax(void* arg, const int key, const int count);
void print_scores(counters_t* scores, char* pageDirectory);
void delete_helper(void* item);
void unionHelper(void* arg, const int key, const int count);
int getMaxDocID(const char* pageDirectory);
static void itemcount(void* arg, const int key, const int count);

Error handling and recovery:
We catch errors with frequent null checks and some are handled internally.

Testing Plan:
Write testing.sh that invokes querier with words that are known and not known, and with bad arguments