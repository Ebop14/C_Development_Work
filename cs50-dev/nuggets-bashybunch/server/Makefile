# Makefile for server program.
#
# Lucas Mazzucchelli - Dartmouth CS50 Nuggets, Fall 2023

# Compiler and linker
CC = gcc
LD = gcc
MAKE = make

# Compiler flags
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)

# Linker flags
LDFLAGS = -L../libcs50 ./common/server_structs.a ./communications/communications.a ./logging/serverlogging.a ./gamestate/gamestate.a ../support/support.a

# Libraries to link with
LDLIBS = -lcs50-given

# Include path
INCLUDES = -I../libcs50 -I./common -I./communications -I./logging -I./gamestate

# Source and binary executable
SRCS = server.c
OBJS = $(SRCS:.c=.o)
EXEC = server

# Dependencies
server.o: ./common/server_structs.h ./communications/communications.h ./logging/serverlogging.h ./gamestate/gamestate.h ../support/message.h ../libcs50/file.h

all: $(EXEC)

# Default rule to build the executable
$(EXEC): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) $(LDLIBS) -lm -o $@

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Phony targets for cleaning and running
.PHONY: clean run

clean:
	rm -f $(OBJS) $(EXEC) *~ core

run: $(EXEC)
	./$(EXEC)
