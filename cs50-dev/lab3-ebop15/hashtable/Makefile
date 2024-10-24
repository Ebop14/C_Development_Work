# Makefile for 'set' module
#
# David Kotz, April 2016, 2017, 2019, 2021
# updated by Xia Zhou, July 2016, 2018, January 2019

OBJS = hashtabletest.o hashtable.o hash.o ../set/set.o ../lib/file.o ../lib/mem.o 
LIBS =

# uncomment the following to turn on verbose memory logging
TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../lib -I../set
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

hashtabletest: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

hashtabletest.o: hashtable.h ../set/set.h ../lib/file.h
hashtable.o: hashtable.h ../set/set.h
../lib/file.o: ../lib/file.h

.PHONY: test valgrind clean

# expects a file `settest.names` to exist; it can contain any text.
test: hashtabletest hashtabletest.names
	./hashtabletest < hashtabletest.names

# expects a file `test.names` to exist; it can contain any text.
valgrind: hashtabletest hashtabletest.names
	$(VALGRIND) ./hashtabletest < hashtabletest.names

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f hashtabletest
	rm -f core