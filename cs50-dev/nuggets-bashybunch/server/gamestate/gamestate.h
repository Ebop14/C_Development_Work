// written by Ethan Child, Lucas Mazzucchelli for CS50
#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "../../libcs50/mem.h"
#include "../../libcs50/file.h"
#include "../../libcs50/hashtable.h"
#include "../../libcs50/counters.h"
#include "../../libcs50/set.h"
#include "../common/server_structs.h"
#include "goldPiles.h"
#include "../../support/message.h"


void pile_item_delete(void* item);

void gamestateDelete(gamestate_t* game);

gamestate_t* initializeGame(FILE* fp);

void updatePlayerPosition(gamestate_t* game, hashtable_t* playerTable, char* player, char message);

void redraw(player_t* player_pointer, gamestate_t* game_pointer);

void checkForGold(player_t* player, gamestate_t* game);

bool checkPoint(player_t* player, gamestate_t* game, int xToCheck, int yToCheck);

bool addPlayer(gamestate_t* game, const char* name, addr_t * endpoint);

bool removePlayer(player_t* player);

bool addSpectator(gamestate_t* game, addr_t* newEndpoint);

#endif
