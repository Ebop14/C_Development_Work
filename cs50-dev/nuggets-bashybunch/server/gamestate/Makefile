# gamestate make file
# written by Ethan Child for CS50
LIB = gamestate.a

OBJS = gamestate.o goldPiles.o
LIBS = ../common/common.a ../../libcs50/libcs50-given.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING)
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)
	ranlib $(LIB)

goldPiles.o: ../../libcs50/counters.h ../../libcs50/hashtable.h  ../logging/serverlogging.h
gamestate.o: gamestate.c goldPiles.h ../../libcs50/mem.h ../../libcs50/file.h ../../libcs50/hashtable.h ../common/server_structs.h ../../libcs50/counters.h ../../libcs50/set.h ../logging/serverlogging.h

.PHONY: test valgrind clean
test:
	bash -v testing.sh >& testing.out
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f $(LIB)
	rm -f core
	rm -f vgcore*
