# Makefile for server logging library.
#
# Lucas Mazzucchelli - Dartmouth CS50 Nuggets, Fall 2023

# Define library
LIB = serverlogging.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

# Include files
INCLUDES = -I../../support

# Source and object files
SRCS = serverlogging.c
OBJS = $(SRCS:.c=.o)

# Build library
$(LIB): $(OBJS)
	ar rcs $@ $^

# Dependencies
DEPS = serverlogging.h ../../support/log.h

# Compilation rule
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean
.PHONY: clean

clean:
	rm -f core
	rm -f $(LIB) *~ *.o
