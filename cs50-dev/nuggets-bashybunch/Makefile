# Makefile for CS50 Nuggets
#
# Lucas Mazzucchelli - Dartmouth CS50 Nuggets, Fall 2023

MAKE = make

.PHONY: all support client server clean

all: support client server

# Build support library
support:
	cd support && $(MAKE)

# Build the client
client:
	cd client && $(MAKE)

# Build the server
server:
	cd server/common && $(MAKE)
	cd server/gamestate && $(MAKE)
	cd server/logging && $(MAKE)
	cd server/communications && $(MAKE)
	cd server && $(MAKE) all

# Clean
clean: clean-client clean-server clean-support

clean-client:
	cd client && $(MAKE) clean

clean-server:
	cd server/common && $(MAKE) clean
	cd server/gamestate && $(MAKE) clean
	cd server/logging && $(MAKE) clean
	cd server/communications && $(MAKE) clean
	cd server && $(MAKE) clean

clean-support:
	cd support && $(MAKE) clean
