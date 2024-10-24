# Makefile for the server's common library.
#
# Lucas Mazzucchelli - Dartmouth CS50 Nuggets, Fall 2023

# Output library
LIB = server_structs.a

# Compiler settings
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
MAKE = make

# Include path
INCLUDES = -I../../libcs50

# Source and object files
SRCS = server_structs.c
OBJS = $(SRCS:.c=.o)

# Header files
DEPS = server_structs.h

# Compilation rule for object files
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build library
$(LIB): $(OBJS)
	ar rcs $@ $^

# Phony targets for cleaning and making library
.PHONY: clean 

clean:
	rm -f core
	rm -f $(LIB) *~ *.o
