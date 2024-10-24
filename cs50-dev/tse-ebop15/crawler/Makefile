# Makefile for crawler

# object files
OBJS = crawler.o hashtable.o webpage.o file.o pagedir.o set.o
LLIBS = ../common/common.a ../libcs50/libcs50-given.a 

PROG = crawler
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

all: crawler

crawler: crawler.o $(LLIBS)
		$(CC) $(CFLAGS) $^ $(LIBS) -o $@
# dependencies
crawler.o: ../libcs50/bag.h ../libcs50/hashtable.h ../libcs50/webpage.h ../libcs50/file.h ../libcs50/mem.h ../common/pagedir.h


test: crawler testing.sh
		bash -v testing.sh &> testing.out

valgrind: crawler testing.sh
	$(VALGRIND) ./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters 3
.PHONY: clean sourcelist

sourcelist: Makefile *.md *.c *.h
	@echo $^

clean:
	rm -f core
	rm -f $(LIB) *~ *.o