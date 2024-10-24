/*
server_structs.c
Implementation of server_structs.h.

Lucas Mazzucchelli - Dartmouth CS50 Nuggets, Fall 2023
*/


#include <stdbool.h>
#include "../../libcs50/hashtable.h"
#include "server_structs.h"
#include "../gamestate/goldPiles.h"
#include "../../support/message.h"


struct position {
    int x;
    int y;
};

struct player {
    char* name;
    char letter;
    position_t* pos;
    int purse;
    addr_t* endpoint;
    char** visibleGrid;
    bool activeStatus;
};

struct spectator {
    addr_t* endpoint;
};

struct gamestate {
    char** grid; // an array of strings, so we can index by row AND column
    int remainingGold; // total gold to be collected
    int numPiles; // number of piles to be collected
    goldPiles_t* piles; // the piles of gold, maps
    int gridWidth; // weidth of the map
    int gridHeight; // height of the map
    hashtable_t* players; // key letter, value player data structure
    spectator_t* activeSpectator;
};
