#!/bin/bash

# one parameter
./querier

# bad parameters
./querier ../data/letters-22 BAD

./querier BAD ../data/toWrite

echo tse | ./querier ../data/letters-22 ../data/toWrite

echo not present

echo and test
 
echo or test

echo test or

echo test and

