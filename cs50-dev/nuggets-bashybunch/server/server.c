/*
server.c
Implementation of the server program.

Starts from the command line receiving a path to a map file and an integer seed for random generation.
Validates arguments.
Initializes logging, messaging, and gamestate modules.
Listens for messages from clients.
Upon game over, terminates modules.

Lucas Mazzucchelli - Dartmouth CS50 Nuggets, Fall 2023
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../libcs50/file.h"
#include "../support/message.h"
#include "common/server_structs.h"
#include "logging/serverlogging.h"
#include "communications/communications.h"
#include "gamestate/gamestate.h"


#define LOGFILE "game.log" // Path to file where game log shall be stored


// Function prototypes
int validateArguments(int argc, char* argv[]);


int main(int argc, char* argv[]) {
    // Validate command line arguments
    int validationCode = validateArguments(argc, argv);
    if (validationCode != 0) {
        printf("\nUSAGE: ./server map.txt [seed]\n");
        exit(validationCode); // Invalid arguments
    }

    // Call srand()
    if (argc == 2) {
        srand((unsigned int)getpid());
    }
    else {
        int seed;
        sscanf(argv[2], "%d", &seed);
        srand((unsigned int)seed);
    }

    // Create pointer to map file
    char* mapFile = argv[1];
    FILE* fpMap = fopen(mapFile, "r");
    if (!fpMap) {
        perror("\nCannot read map file.\n");
        exit(5); // Invalid map file
    }

    // Create pointer to log file
    FILE* fpLog = fopen(LOGFILE, "w");
    if (!fpLog) {
        perror("\nCannot write to log file.\n");
        fclose(fpMap);
        exit(6); // Invalid log file
    }

    // Initialize logging module
    serverlogInit(fpLog);

    // Initialize communications module
    int port = message_init(fpLog);
    if (port == 0) {
        fprintf(stderr, "\nCannot initialize communications.\n");
        serverlogDone();
        fclose(fpMap);
        fclose(fpLog);
        exit(7); // Communications initialization failure
    }

    // Initialize gamestate module
    gamestate_t* game = initializeGame(fpMap);
    if (!game) {
        fprintf(stderr, "\nCannot initialize game.\n");
        serverlogDone();
        message_done();
        fclose(fpMap);
        fclose(fpLog);
        exit(8); // Game intialization failure
    }

    // Listen
    communications_listen(game);

    // Terminate modules
    gamestateDelete(game);
    serverlogDone();
    message_done();

    // Close file pointers
    fclose(fpMap);
    fclose(fpLog);

    exit(0); // Success
}


/*
Helper function to validate command line arguments.
*/
int validateArguments(int argc, char* argv[]) {
    // Validate argument count
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "\nERROR: Invalid argument count.\n");
        return 1;
    }

    // Validate map filepath by checking extension
    const char* mapFilepath = argv[1];
    const char* extension = strrchr(mapFilepath, '.'); // Find the last occurrence of '.'
    if (!extension || strcmp(extension, ".") == 0 || strcmp(extension, mapFilepath) == 0) {
        fprintf(stderr, "\nERROR: Invalid path to map file.\n");
        return 2;
    }
    if (strcmp(extension, ".txt") != 0) {
        fprintf(stderr, "\nERROR: Invalid map file extension.\n");
        return 3;
    }

    // If seed given, validate it
    if (argc == 3) {
        int seed;
        if (sscanf(argv[2], "%d", &seed) != 1 || seed <= 0) {
        fprintf(stderr, "\nERROR: Invalid seed.\n");
        return 4;
        }
    }

    return 0; // Arguments are valid
}
