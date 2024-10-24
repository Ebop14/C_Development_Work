/*
server_structs.h
A common library of types used by the server's modules.

Lucas Mazzucchelli - Dartmouth CS50 Nuggets, Fall 2023
*/


#define MaxNameLength 50   // max number of chars in playerName
#define MaxPlayers 26      // maximum number of players
#define GoldTotal 250      // amount of gold in the game
#define GoldMinNumPiles 10 // minimum number of gold piles
#define GoldMaxNumPiles 30 // maximum number of gold piles


#ifndef _SERVER_STRUCTS_H_
#define _SERVER_STRUCTS_H_


// Forward declarations
typedef struct position position_t;
typedef struct player player_t;
typedef struct spectator spectator_t;
typedef struct gamestate gamestate_t;


#endif // _SERVER_STRUCTS_H_
