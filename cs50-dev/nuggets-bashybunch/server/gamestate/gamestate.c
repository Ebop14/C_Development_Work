/*
gamestate.c
Implementation of the server's gamestate module.

Ethan Child, Lucas Mazzucchelli - Dartmouth CS50 Nuggets, Fall 2023
*/


#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "gamestate.h"
#include "../common/server_structs.h"
#include "../common/server_structs.c"
#include "../logging/serverlogging.h"


// Function prototypes
char* intToString(int i);
static void iteratePlayers(void* arg, const char* key, void* item);
static void checkCoords(void* arg, const char* key, void* player);
char* truncateName(const char* name);
int countPlayers(hashtable_t* ht);
void incrementCount(void* arg, const char* key, void* item);
char getLetter();
position_t* generateStartingPosition(char** grid, int gridWidth, int gridHeight);
void freePlayer(void* item);
void freeSpectator(void* item);
static bool checkMove(int x, int y, char ** grid, int width, int height);

// deletes the gamestate 
void gamestateDelete(gamestate_t* game) {
    logMessage("Deleting gamestate...");

    logMessage("Freeing grid...");
    // Free grid
    if (game->grid) {
        for (int i = 0; i < game->gridHeight; i++) {
            if (game->grid[i]) {
                free(game->grid[i]); // Free each row
            }
        }
        free(game->grid); // Free the array of rows
        game->grid = NULL;
    }

    // Free other attributes
    logMessage("Freeing players hashtable...");
    hashtable_delete(game->players, freePlayer);

    logMessage("Freeing spectator...");
    freeSpectator(game->activeSpectator);

    logMessage("Freeing piles hashtable...");
    piles_delete(game->piles);

    // Free gamestate pointer
    free(game);
    logMessage("Gamestate deleted.");
}


// initialize the game
gamestate_t* initializeGame(FILE* fp) {
    logMessage("Initializing game...");
    
    // allocate the memory for the game
    logMessage("Allocating memory for gamestate...");
    gamestate_t* game = mem_malloc(sizeof(gamestate_t));
    if (!game) {
        logError("Failed to allocate memory for gamestate.");
        return NULL;
    }
   
    char* mapString = file_readFile(fp);

    int rowCount = 0;
    int columnCount = 0;
    int maxColumnCount = 0;

    // First, count the number of rows and the maximum column count
    for (int i = 0; mapString[i] != '\0'; i++) {
        if (mapString[i] == '\n') {
            rowCount++;
            maxColumnCount = (columnCount > maxColumnCount) ? columnCount : maxColumnCount;
            columnCount = 0; // Reset for the next row
        } 
        else {
            columnCount++;
        }
    }

    // Allocate memory for each row
    logMessage("Allocating memory for grid rows...");
    game->grid = malloc(sizeof(char*) * rowCount);
    if (!game->grid) {
        logError("Failed to allocate memory for game grid rows.");
        gamestateDelete(game);
        return NULL;
    }

    // Allocate memory for each column in each row
    logMessage("Allocating memory for grid columns...");
    for (int r = 0; r < rowCount; r++) {
        game->grid[r] = malloc(sizeof(char) * (maxColumnCount + 1)); // +1 for the null terminator
        if (!game->grid[r]) {
            logError("Failed to allocate memory for grid column.");
            gamestateDelete(game);
            return NULL;
        }
    }

    int numEmptySpots = 0; // Tracks the number of empty positions on the grid

    // Copy the characters from the mapString to the 2D grid
    int r = 0, c = 0;
    for (int i = 0; mapString[i] != '\0'; i++) {
        if (mapString[i] == '\n') {
            game->grid[r][c] = '\0'; // Null-terminate the current row
            r++;
            c = 0;
        } 
        else {
            if (mapString[i] == '.') {
                numEmptySpots++;
            }

            game->grid[r][c] = mapString[i];
            c++;
        }
    }

    game->gridHeight = rowCount;
    game->gridWidth = maxColumnCount;

    // Create players hashtable
    logMessage("Allocating memory for players hashtable...");
    game->players = hashtable_new(MaxPlayers);
    if (!game->players) {
        logError("Failed to create players hashtable.");
        gamestateDelete(game);
        return NULL;
    }

    // Set spectator
    game->activeSpectator = NULL;

    // Set remainingGold to initial total gold supply
    game->remainingGold = GoldTotal;

    // Build a list of all empty positions on the grid
    position_t* emptySpots = malloc(numEmptySpots * sizeof(position_t));
    if (!emptySpots) {
        logError("Failed to allocate memory for array of empty positions.");
        gamestateDelete(game);
        return NULL;
    }
    int index = 0;
    for (int y = 0; y < game->gridHeight; y++) {
        for (int x = 0; x < game->gridWidth; x++) {
            if (game->grid[y][x] == '.') {
                emptySpots[index++] = (position_t){x, y};
            }
        }
    }

    // Shuffle the list of empty positions
    for (int i = numEmptySpots - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        position_t temp = emptySpots[i];
        emptySpots[i] = emptySpots[j];
        emptySpots[j] = temp;
    }

    // Determine number of gold piles
    game->numPiles = GoldMinNumPiles;
    int upperLimit = (numEmptySpots < GoldMaxNumPiles) ? numEmptySpots : GoldMaxNumPiles;
    if (upperLimit > GoldMinNumPiles) {
        // Generate a random number of piles between minPiles and upperLimit
        game->numPiles = (rand() % (upperLimit - GoldMinNumPiles + 1)) + GoldMinNumPiles;
    } 

    // Assign number of gold piles
    logMessage("Allocating memory for piles attribute...");
    game->piles = piles_new(game->numPiles);
    if (!game->piles) {
        logError("Failed to create goldPiles hashtable.");
        gamestateDelete(game);
        free(emptySpots);
        return NULL;
    }

    logMessage("Creating gold piles...");
    int amountAdded = 0;
    for (int i = 0; i < game->numPiles; i++) {
        if(250 <= amountAdded){
            break;
        }
        int remaining = 250 - amountAdded;
        printf("%d\n", remaining);
        
        int avg = remaining / (game->numPiles - i);
        int amount = (int) (rand() % (avg - 1)) + 1; // Ensure at least 1 gold

        position_t spot = emptySpots[i];
        char* goldX_str = intToString(spot.x);

        if (!piles_add(game->piles, goldX_str, spot.y, amount)) {
            logError("Failed to insert into piles hashtable.");
            free(goldX_str);
            free(emptySpots);
            gamestateDelete(game);
            return NULL;
        }

    free(goldX_str);
    game->grid[spot.y][spot.x] = '*'; // Mark the grid spot with gold
    amountAdded += amount;
    }

    free(emptySpots);

    logMessage("Successfully initialized gamestate.");
    return game;
}


void updatePlayerPosition(gamestate_t* game, hashtable_t* playerTable, char* player, char message){
    logFormattedString("Updating position of player '%s'.", player);

    // Find player in hashtable
    player_t* mover = hashtable_find(playerTable, player);
    if (!mover) {
        logError("Cannot find player in hashtable.");
        return;
    }

    // Store current position
    char ** grid = game->grid;
    int moverX = mover->pos->x;
    int moverY = mover->pos->y;
    int width = game->gridWidth;
    int height = game->gridHeight;

    // Check message
    if(message == 'h'){ // left
        // collision checks
        if(!checkMove(moverX-1, moverY, grid, width, height)){ // if collided
            logMessage("Player performed invalid movement.");
            return;
        }
        // not moving into a wall
        mover->pos->x -= 1;

        // check if the new position contains gold
        checkForGold(mover, game);
           
        // check whether the position is unoccupied  LAST
        hashtable_iterate(game->players, mover, checkCoords);
    }
    if(message == 'l'){ // right
        // collision checks
        if(!checkMove(moverX+1, moverY, grid, width, height)){ // if collided
            logMessage("Player performed invalid movement.");
            return;
        }            
        // move
        mover->pos->x += 1;

        // check if the new position contains gold
        checkForGold(mover, game);
           
        // check whether the position is unoccupied  LAST
        hashtable_iterate(game->players, mover, checkCoords);
    }
    if(message == 'j'){ // down
        // collision checks
        if(!checkMove(moverX, moverY+1, grid, width, height)){ // if collided
            logMessage("Player performed invalid movement.");
            return;
        }       
        // move
        mover->pos->y += 1;

        // check if the new position contains gold
        checkForGold(mover, game);
            
        // check whether the position is unoccupied  LAST
        hashtable_iterate(game->players, mover, checkCoords);
    }
    if(message == 'k'){ // up
        // collision checks
        if(!checkMove(moverX, moverY-1, grid, width, height)){ // if collided
            logMessage("Player performed invalid movement.");
            return;
        }        
        // move
        mover->pos->y -= 1;
        
        // check if the new position contains gold
        checkForGold(mover, game);
            
        // check whether the position is unoccupied  LAST
        hashtable_iterate(game->players, mover, checkCoords);
    }
    if(message == 'y'){ // up and left
        // collision checks
        if(!checkMove(moverX-1, moverY-1, grid, width, height)){ // if collided
            logMessage("Player performed invalid movement.");
            return;
        }
                
        // move
        mover->pos->y -= 1;
        mover->pos->x -= 1;
        
        // check if the new position contains gold
        checkForGold(mover, game);
            
        // check whether the position is unoccupied  LAST
        hashtable_iterate(game->players, mover, checkCoords);
    }
    if(message == 'u'){ // up and right
        // collision checks
        if(!checkMove(moverX+1, moverY-1, grid, width, height)){ // if collided
            logMessage("Player performed invalid movement.");
            return;
        }        
        // move
        mover->pos->y -= 1;
        mover->pos->x += 1;

        // check if the new position contains gold
        checkForGold(mover, game);
            
        // check whether the position is unoccupied  LAST
        hashtable_iterate(game->players, mover, checkCoords);
    }
    if(message == 'b'){ // down and left
        // collision checks
        if(!checkMove(moverX-1, moverY+1, grid, width, height)){ // if collided
            logMessage("Player performed invalid movement.");
            return;
        }
        // move
        mover->pos->y += 1;
        mover->pos->x -= 1;
        
        // check if the new position contains gold
        checkForGold(mover, game);
            
        // check whether the position is unoccupied  LAST
        hashtable_iterate(game->players, mover, checkCoords);       
    }
    if(message == 'n'){ // down and right
        // collision checks
        if(!checkMove(moverX+1, moverY+1, grid, width, height)){ // if collided
            logMessage("Player performed invalid movement.");
            return;
        }
        // move
        mover->pos->y += 1;
        mover->pos->x += 1;
        
        // check if the new position contains gold
        checkForGold(mover, game);
            
        // check whether the position is unoccupied  LAST
        hashtable_iterate(game->players, mover, checkCoords);
    }

    if (grid[moverY][moverX] == player[0]) {
        grid[moverY][moverX] = '.';
        grid[mover->pos->y][mover->pos->x] = player[0];
    }
}


static bool checkMove(int x, int y, char ** grid, int width, int height) {
    if (x > width-1 || x < 0) {
        return(false);
    }  

    if (y >height-1 || y < 0) {
        return(false);
    }

    if (grid[y][x] == '-' || grid[y][x] == '|' || grid[y][x] == '+' || grid[y][x] == ' ' ) {
        return(false);
    }
    return(true);
}

void redraw(player_t* player_pointer, gamestate_t* game_pointer) {
    gamestate_t* game = game_pointer;
    player_t* player = player_pointer;
    logMessage("Game and player copied.");

    hashtable_iterate(game->players, game, iteratePlayers);
    logMessage("Checked for interference.");

    if(game->grid[player->pos->y][player->pos->x] == '*'){
        logMessage("Gold found.");

        char* x_string = intToString(player->pos->y);
        char* y_string = intToString(player->pos->x);

        int gold = counters_get(piles_find(game->piles, x_string), player->pos->x);
        player->purse += gold;
        counters_set(piles_find(game->piles, x_string), player->pos->x, 0);
        mem_free(x_string);
        mem_free(y_string);
        logMessage("Gold retrieved.");
    }

    // piles_iterate(game->piles, &player, iterateGold);

    logMessage("Preparing to create visibility grid.");
    int x = 0;
    while(x < game->gridWidth){
        int y = 0;
        while(y < game->gridHeight){
            if(game->grid[x][y] == '*'){
                char* x_string = intToString(x);
                char* y_string = intToString(y);
                
                if(counters_get(hashtable_find(game->piles, x_string), y) == 0){
                    mem_free(x_string);
                    mem_free(y_string);
                    continue;
                }
            }
            if(!checkPoint(player_pointer, game_pointer, x, y)){
                player->visibleGrid[x][y] = ' ';
            }
            else{
                player->visibleGrid[x][y] = game->grid[x][y];
            }

            // player->visibleGrid[x][y] = game->grid[x][y];
            y += 1;
        }
        x += 1;
    }
    logMessage("Visibility grid created.");
        // grid that we're seeing right now (updating the visible grid we've seen)
        
        // grid of players and gold we're seeing rn
}


static void iteratePlayers(void* arg, const char* key, void* item){
    logMessage("Checking a player.");
     
    if(arg != NULL && key != NULL && item != NULL){
        gamestate_t* game = (gamestate_t *)arg;
        player_t* player = item;
        logMessage("Game and player copied.");
        if(player->activeStatus){
            // Temp variables
            game->grid[player->pos->y][player->pos->x] = player->letter;
            logMessage("Positions compared.");
        }
    }
}


static void checkCoords(void* arg, const char* key, void* player) {
    player_t* player1 = arg;
    player_t* player2 = player;

    // NULL checks
    if (!player1 || !player2) {
        logError("checkCoords: received NULL player.");
        return;
    }

    // Swap coordinates of players if there is a collision
    if (player1->pos->x == player2->pos->x && player1->pos->y == player2->pos->y) {
        logMessage("Identified collision.");
        logFormattedChar("Player %c collided.", player1->letter);
        logFormattedChar("Player %c collided.", player2->letter);
        logMessage("Swapping player coordinates...");

        int temp_x;
        int temp_y;

        temp_x = player2->pos->x;
        temp_y = player2->pos->y;

        player2->pos->x = player1->pos->x;
        player2->pos->y = player1->pos->y;

        player1->pos->x = temp_x;
        player1->pos->y = temp_y;
    }
}


bool checkPoint(player_t* player, gamestate_t* game, int xToCheck, int yToCheck) {
    // NULL checks
    if (!player) {
        logError("checkPoint: received NULL player.");
        return false;
    }
    if (!game) {
        logError("checkPoint: received NULL gamestate.");
        return false;
    }

    int dx = xToCheck - player->pos->x;
    int dy = yToCheck - player->pos->y;
    double slope = (double)((double) dx / (double) dy);

    int origin = player->pos->y;

    while (abs(origin) < abs(xToCheck)) {
        double newX = origin;
        double newY = slope * origin;

        if (round(newX) == newX && round(newY) == newY) {
            int int_newX = round(newX);
            int int_newY = round(newY);
            if (game->grid[int_newY][int_newX] != '.' && game->grid[int_newY][int_newX] != '*') {
                return false;
            }
        }
        else {
            int left_newX = round(newX - 0.5);
            int right_newX = round(newX + 0.5);
            int int_newY = round(newY);

            if (game->grid[int_newY][left_newX] != '.' && game->grid[int_newY][left_newX] != '*' && game->grid[int_newY][right_newX] != '.' && game->grid[int_newY][right_newX] != '*') {
                return false;
            }
        }

        if (dx < 0) {
            origin -= 1;
        }
        else {
            origin += 1;
        }
    }

    return true;
}


void checkForGold(player_t* player, gamestate_t* game) {
    // NULL checks
    if (!player) {
        logError("checkForGold: NULL player received.");
        return;
    }
    if (!game) {
        logError("checkforGold: NULL gamestate received.");
        return;
    }

    char spotToCheck = game->grid[player->pos->y][player->pos->x];
    if (spotToCheck == '*') {
        logMessage("Gold found.");
        char* x_string = intToString(player->pos->x);
        char* y_string = intToString(player->pos->y);
        if (!x_string || !y_string) {
            return;
        }

        // Retrieve gold pile
        int gold = counters_get(piles_find(game->piles, x_string), player->pos->y);
        if (gold == 0) {
            logError("Failed to get gold pile counter.");
            mem_free(x_string);
            mem_free(y_string);
            return;
        }

        logFormattedString("Updating purse of '%s'...", player->name);
        player->purse += gold;// whatever gold is on the *
        logMessage("Updating remaining gold...");
        game->remainingGold -= gold; // whatever the gold above was
        
        mem_free(x_string);
        mem_free(y_string);
    }
}


char* intToString(int i) {
    // Find length of docID
    int toReturn = i;
    int size = 1;
    // include case for 0
    if (i == 0) {
        size++;
    }
    while (toReturn > 0) {
        toReturn = toReturn/10;
        size++;
    }
    // Convert docID to a string
    char* string = mem_malloc(size);
    if (!string) {
        logError("Failed to convert int to string.");
        return NULL;
    }
    sprintf(string, "%d", toReturn);
    return string;
}


/*
Adds a player to the game.
*/
bool addPlayer(gamestate_t* game, const char* name, addr_t * endpoint) {
    logMessage("Received request to add player.");

    // Check that player limit has not been reached
    logMessage("Checking for room in lobby...");
    if (countPlayers(game->players) >= MaxPlayers) {
        logError("Cannot add player: max players reached.");
        return false;
    }

    // Create a new player_t instance and allocate memory
    logMessage("Allocating memory for new player...");
    player_t* newPlayer = (player_t*)malloc(sizeof(player_t));
    if (!newPlayer) {
        logError("Cannot add player: memory allocation failed.");
        return false;
    }
    
    // Truncate player name
    logMessage("Truncating player name...");
    char* truncatedName = truncateName(name);
    if (!truncatedName) {
        logError("Cannot add player: name truncation failed.");
        freePlayer(newPlayer);
        return false;
    }
    newPlayer->name = truncatedName;

    // Generate letter
    logMessage("Assigning letter...");
    char playerLetter = getLetter();
    newPlayer->letter = playerLetter;

    // Generate starting position
    logMessage("Generating spawn position...");
    position_t* startingPosition = generateStartingPosition(game->grid, game->gridWidth, game->gridHeight);
    if (!startingPosition) {
        logError("Cannot add player: position generation failed.");
        freePlayer(newPlayer);
        return false;
    }
    newPlayer->pos = startingPosition;
    int x = startingPosition->x;
    int y = startingPosition->y;
    game->grid[y][x] = playerLetter;

    // Assign endpoint
    logMessage("Assigning endpoint...");
    newPlayer->endpoint = endpoint;

    // Initialize remaining attributes
    logMessage("Initializing purse...");
    newPlayer->purse = 0;
    logMessage("Assigning active status...");
    newPlayer->activeStatus = true;

    newPlayer->visibleGrid = NULL;

    // Allocate memory for hashtable key
    logMessage("Creating hashtable key...");
    char* playerKey = malloc(2);
    if (!playerKey) {
        logError("Cannot add player: failed to allocate memory for player's hashtable key.");
        freePlayer(newPlayer);
        return false;
    }
    playerKey[0] = playerLetter;
    playerKey[1] = '\0';

    // Insert player into players hashtable
    logMessage("Inserting into players hashtable...");
    if (!hashtable_insert(game->players, playerKey, newPlayer)) {
        logError("Cannot add player: failed to insert into players hashtable.");
        freePlayer(newPlayer);
        free(playerKey);
        return false;
    }

    // Call redraw for new player
    //logMessage("Getting visible grid...");
    //redraw(newPlayer, game);
    newPlayer->visibleGrid = game->grid;

    logFormattedString("Added player '%s'.", truncatedName);
    logFormattedChar("Assigned letter '%c'.", playerLetter);

    return true; // Success
}


/*
Helper function to truncate player name.
*/
char* truncateName(const char* name) {
    if (!name) {
        logError("Unable to truncate NULL name.");
        return NULL;
    }

    // Allocate memory for the truncated name
    char* truncatedName = malloc(MaxNameLength + 1); // +1 for the null terminator
    if (!truncatedName) {
        logError("Unable to allocate memory for truncated name.");
        return NULL;
    }

    // Copy and truncate the name to MaxNameLength characters
    strncpy(truncatedName, name, MaxNameLength);
    truncatedName[MaxNameLength] = '\0'; // Ensure null termination

    // Replace non-graphical and non-blank characters with '_'
    for (int i = 0; truncatedName[i] != '\0'; i++) {
        if (!isgraph(truncatedName[i]) && !isblank(truncatedName[i])) {
            truncatedName[i] = '_';
        }
    }

    return truncatedName;
}


/*
Helper function to addPlayer() that counts number of keys in players hashtable.
*/
int countPlayers(hashtable_t* ht) {
    int count = 0;
    hashtable_iterate(ht, &count, incrementCount);
    return count;
}


/*
Helper function to countPlayers() that serves as itemfunc to hashtable_iterate().
*/
void incrementCount(void* arg, const char* key, void* item) {
    int* count = (int*) arg;

    if (count != NULL) {
        (*count)++;
    }
}


/*
Helper function to generate a unique letter for a new player.
*/
char getLetter() {
    static char nextLetter = 'A';
    return nextLetter++;
}


/*
Helper function to addPlayer() that randomly generates player starting position.
*/
position_t* generateStartingPosition(char** grid, int gridWidth, int gridHeight) {
    // Build a list of all empty positions on the grid
    position_t* emptySpots = malloc(gridWidth * gridHeight * sizeof(position_t));
    if (!emptySpots) {
        logError("Failed to allocate memory for array of empty positions.");
        return NULL;
    }
    int index = 0;
    for (int y = 0; y < gridHeight; y++) {
        for (int x = 0; x < gridWidth; x++) {
            if (grid[y][x] == '.') {
                emptySpots[index++] = (position_t){x, y};
            }
        }
    }

    // Check if there are any empty spots
    if (index <= 0) {
        logError("No empty spots found for new player spawn.");
        free(emptySpots);
        return NULL;
    }

    // Randomly select an empty spot
    int randomIndex = rand() % index;

    // Allocate memory for new player's position
    position_t* pos = malloc(sizeof(position_t));
    if (!pos) {
        logError("Memory allocation for new player's position failed.");
        free(emptySpots);
        return NULL;
    }

    // Assign the coordinates of the random empty spot to player's position
    pos->x = emptySpots[randomIndex].x;
    pos->y = emptySpots[randomIndex].y;

    free(emptySpots);
    return pos;
}


/*
Function to set a player as inactive.
*/
bool removePlayer(player_t* player) {
    logMessage("Received request to remove player.");

    // NULL check
    if (!player) {
        logError("Cannot remove NULL player.");
        return false;
    }

    // Validate current status
    if (player->activeStatus) {
        player->activeStatus = false;
        logFormattedString("Removed player '%s'.", player->name);
    }
    else {
        logFormattedString("Player '%s' is already inactive.", player->name);
    }

    return true; // Success
}


/*
Function to add a spectator to the game state.
*/
bool addSpectator(gamestate_t* game, addr_t* newEndpoint) {
    logMessage("Received request to add spectator.");

    // NULL checks
    if (!game) {
        logError("Received null gamestate.");
    }
    if (newEndpoint) {
        logError("Received NULL endpoint.");
    }

    logFormattedString("Addind spectator with endpoint '%s'.", (char *)message_stringAddr(*newEndpoint));

    // Create new Spectator instance
    spectator_t* newSpectator = (spectator_t*)malloc(sizeof(spectator_t));
    if (!newSpectator) {
        logError("Cannot add spectator: memory allocation failed.");
        return false;
    }

    // If there is an active spectator, remove them
    if (game->activeSpectator) {
        freeSpectator(game->activeSpectator);
        logMessage("Removed previous spectator.");
    }

    newSpectator->endpoint = newEndpoint;

    // Add new spectator
    game->activeSpectator = newSpectator;
    logFormattedString("Added new spectator with endpoint '%s'.", (char *)message_stringAddr(*newEndpoint));
    
    return true; // Success
}


/*
Helper function to free a player instance.
*/
void freePlayer(void* item) {
    player_t* player = item;

    if (player) {
        if (player-> name) {
            free(player->name);
        }
        if (player->endpoint) {
            free(player->endpoint);
        }
        if (player->pos) {
            free(player->pos);
        }
        if (player->visibleGrid) {
            free(player->visibleGrid);
        }
        
        free(player);
    }
}


/*
Helper function to free a spectator instance.
*/
void freeSpectator(void* item) {
    spectator_t* spectator = item;

    if (spectator) {
        if (spectator->endpoint) {
            free(spectator->endpoint);
        }
        
        free(spectator);
    }
}
