
README file for gamestate data structure

This data structure holds the information to run the game.

Aspects of the data structure: 
* grid: A 2D array of char characters. Represents the map.
* remainingGold: The total gold (amount, not piles) on the map
* numPiles: The number of gold piles on the map
* piles: A hashtable wrapper that maps the x coordinate to a counters object that maps a y coordinate to the amount of gold at the (x, y) coordinate
* gridWidth: The width of the map
* gridHeight: The height of the map
* players: A hashtable that maps the player's letter to their player data structure
* activeSpectator: holds the spectator data structure for the current active spectator

Functions: 

* char* intToString(int i): a helper function that converts integers to strings

void pile_item_delete(void* item): item delete function for pile

void gamestateDelete(gamestate_t* game): deletes the gamestate

gamestate_t* initializeGame(char* textFile): initializes the gamestate from the map file

void updatePlayerPosition(gamestate_t* game, hashtable_t* playerTable, char* player, char message): updates the player's position based on the gamestate, their position, and the message passed through to the server

void redraw(player_t** player_pointer, gamestate_t** game_pointer): takes a pointer to the player and a pointer to the game and redraws the player's visibility map

static void iteratePlayers(void* arg, const char* key, void* item): helper that iterates through every player

static void checkCoords(void* arg, const char* key, void* player): checks the coordinates of a player that has just moved, and checks whether our player has collided with another player 

void checkForGold(player_t** player, gamestate_t** game): checks to see if a player has moved to a spot that has gold

bool checkPoint(player_t** player_ptr, gamestate_t** game_ptr, int xCheck, int yCheck): given a player pointer, a game, and two coordinates to check, checks whether our player can see the given coordinate

bool addPlayer(gamestate_t* game, const char* name, const char* endpoint): adds a player to the game

bool removePlayer(gamestate_t* game, char player_letter): removes a player from the game

bool addSpectator(gamestate_t* game, const char* newEndpoint): adds a spectator to the game