# Makefile for indexer

# object files
OBJS = querier.o bag.o word.o hashtable.o file.o mem.o pagedir.o index.o
LLIBS = ../common/common.a ../libcs50/libcs50-given.a 

PROG = querier
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

all: querier

querier: querier.o $(LLIBS)
		$(CC) $(CFLAGS) $^ $(LIBS) -o $@ 

# dependencies

querier.o: ../libcs50/bag.h ../common/word.h ../libcs50/hashtable.h ../libcs50/file.h \
../libcs50/mem.h ../common/pagedir.h ../common/index.h

test: querier testing.sh
		bash -v testing.sh &> testing.out

valgrind: querier #testing.sh
#$(VALGRIND) test

	$(VALGRIND) ./querier ../data/letters-22 ../data/toWrite
.PHONY: clean sourcelist

sourcelist: Makefile *.md *.c *.h
	@echo $^

clean:
	rm -f core
	rm -f $(LIB) *~ *.o