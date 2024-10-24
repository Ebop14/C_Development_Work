# Makefile for common

# object files
OBJS = pagedir.o index.o word.o
LIB = common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

# dependencies
pagedir.o: pagedir.h ../libcs50/file.h ../libcs50/mem.h

index.o: index.h ../libcs50/file.h ../libcs50/set.h ../libcs50/hashtable.h ../libcs50/counters.h ../libcs50/mem.h

word.o: word.h ../libcs50/mem.h

.PHONY: clean sourcelist

sourcelist: Makefile *.md *.c *.h
	@echo $^

clean:
	rm -f core
	rm -f $(LIB) *~ *.o