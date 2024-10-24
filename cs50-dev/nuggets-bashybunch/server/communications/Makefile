# Makefile for server's communications module.
#
# Lucas Mazzucchelli - Dartmouth CS50 Nuggets, Fall 2023

# Define library
LIB = communications.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

# Include files
INCLUDES = -I../../libcs50 -I../common -I../logging -I../../support -I../gamestate

# Source and object files
SRCS = communications.c
OBJS = $(SRCS:.c=.o)

# Build library
$(LIB): $(OBJS)
	ar rcs $@ $^

# Dependencies
DEPS = communications.h ../../libcs50/hashtable.h ../../support/message.h ../../libcs50/mem.h ../common/server_structs.h ../logging/serverlogging.h ../gamestate/gamestate.h ../gamestate/goldPiles.h

# Compilation rule
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean
.PHONY: clean

clean:
	rm -f core
	rm -f $(LIB) *~ *.o
