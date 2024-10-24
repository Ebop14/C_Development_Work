# Makefile for 'set' module
#
# David Kotz, April 2016, 2017, 2019, 2021
# updated by Xia Zhou, July 2016, 2018, January 2019

OBJS = counterstest.o counters.o ../lib/file.o ../lib/mem.o 
LIBS =

# uncomment the following to turn on verbose memory logging
TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../lib
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

counterstest: $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

counterstest.o: counters.h ../lib/file.h
counters.o: counters.h
../lib/file.o: ../lib/file.h

.PHONY: test valgrind clean

# expects a file `settest.names` to exist; it can contain any text.
test: counterstest counterstest.names
	./counterstest < counterstest.names

# expects a file `test.names` to exist; it can contain any text.
valgrind: counterstest counterstest.names
	$(VALGRIND) ./counterstest < counterstest.names

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f counterstest
	rm -f core