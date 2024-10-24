# Makefile for indexer

# object files
OBJS = indextest.o indexer.o bag.o index.o hashtable.o webpage.o file.o mem.o pagedir.o word.o
LLIBS = ../common/common.a ../libcs50/libcs50-given.a 

PROG = indexer indextest
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

all: indexer indextest

indexer: indexer.o $(LLIBS)
		$(CC) $(CFLAGS) $^ $(LIBS) -o $@ 

indextest: indextest.o $(LLIBS)
		$(CC) $(CFLAGS) $^ $(LIBS) -o $@ 

# dependencies
indexer.o: ../libcs50/bag.h ../common/index.h ../libcs50/hashtable.h \
../libcs50/webpage.h ../libcs50/file.h ../libcs50/mem.h ../common/pagedir.h ../common/word.h

indextest.o: ../common/index.h ../libcs50/file.h

test: indexer testing.sh
		bash -v testing.sh &> testing.out

valgrind: indexer testing.sh
#$(VALGRIND) test

	$(VALGRIND) ./indexer ../data/letters-22 ../data/toWrite
.PHONY: clean sourcelist

sourcelist: Makefile *.md *.c *.h
	@echo $^

clean:
	rm -f core
	rm -f $(LIB) *~ *.o