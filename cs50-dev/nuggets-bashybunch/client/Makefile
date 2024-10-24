# Makefile for client

L = ../support

OBJS = client.o $(L)/message.o
LIBS = $(L)/support.a
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../support
LDLIBS = -lncurses
CC = gcc
MAKE = make

client: $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) $(LLIBS) $(LIBS) $(LDLIBS) -o $@

client.o: client.c client.h $(L)/message.h

.PHONY: all clean

# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

all: $(LIBS) client

clean:
	rm -f *~ *.o
	rm -rf *.dSYM
	rm -f client
