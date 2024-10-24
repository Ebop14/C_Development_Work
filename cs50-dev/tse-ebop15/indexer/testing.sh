#!/bin/bash

# one argument
./indexer

# bad pagedir
./indexer NULL ../data/letters.index


# new index
-rm ../data/abc

./indexer ../data/letters-22 ../data/abc

# good arguments
./indexer ../data/letters-22 ../data/writeTo

# call on toScrape
./indexer ../data/toscrape-1 ../data/scrapeWrite

# call on wikipedia
./indexer ../data/wikipedia-2 ../data/wikiWrite
