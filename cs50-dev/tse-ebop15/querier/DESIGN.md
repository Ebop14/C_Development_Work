Parts to the design spec:
* UI
* Inputs and Outputs
* Functional Decomposition into Modules
* Psuedocode
* Major Data Structures
* Testing Plan

User Interface
Must have three arguments:
querier pageDirectory indexFilename

Inputs and Outputs
Inputs: A crawler-produced pageDirectory,an indexer-produced indexfile, and queries from stdin
Outputs: A ranking of the docIDs, sorted by score, according to the query and produced from indexfile

Functional Decomposition into Modules
Use the following functions:
1. main: parses stdin, then calls build_score and print_scores
2. build_score: builds a list of scores from stdin and the pagedirectory
3. subgroup_intersection: find the intersection of all words within a subgroup
4. get_max_docID: gets the maximum docID. This is global!
5. clean_line: cleans the line from stdin
6. print_scores: prints the scores of all documents, in order

Pseudocode:
Parse the command line for pageDirectory and indexFileName

Wait for input from stdin
    clean the input from stdin
        build the score from the page directory
    print the built score

Major Data Structures:
* Bag: stores words in our sentence
* Counters: used for a variety of things
* Index: parsed from indexFileName, contains docID and word appearances

Testing Plan:
Write testing.sh that invokes querier with words that are known and not known, and with bad arguments