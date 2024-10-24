Parts to the Design Spec:
    Data Structures
    Control Flow
    Detailed Protoype Functions and Parameters
    Error handling and recovery
    Testing Plan



Data Structures
    1. index: a wrapper for a hashtable. It is a hashtable with counters as the items, which allows us to call counter functions on the index's items. 
    2. pageDir (modified): the same as the regular pageDir data structure. We have added a new function, pagedir_load

Control Flow: 
The Main function parses the arguments, calls index_build, and returns 0

index_build
    given a crawled pageDirectory and a target file:
        parses the pageDirectory's files   
            call index_save on the file

index_save
    given an index and a file
        iterate through every item in the index and write to the file

Functional Decomposition into Modules
I used the following modules/functions:
    1. main: parses the arguments and calls indexBuild
    2. index_page: scans an existing index file and creates an index from it. 
    3. index_build: writes to an index file from a given pageDirectory of crawler-produced files
    4. normalizeword: takes the word in and converts all letters to lowercase
    5. index_save: a function in the index data structure that takes an index and a file to write to as parameters and writes the index to the file.
    6. index_load: a function in the index data structure that takes an index file and builds an index from it
    7. pagedir_load: takes a crawled file as a parameter and returns a webpage built from the file

Function Prototypes:
int index_page(index_t* index, webpage_t* page, const int docID)

int index_build(const char* pageDirectory, char* writeTo)

char* normalizeWord(char* word)

int index_save(index_t* index, char* file)

index_t* index_load(char* file_name)

webpage_t* pagedir_load(char* filePath)

Error handling and recovery:
    we use defensive programming and null checks to exit if a function ever receives bad parameters

    some errors are caught internally, as seen when pagedir_load will return NULL if the webpage cannot be built from the given file

Testing Plan