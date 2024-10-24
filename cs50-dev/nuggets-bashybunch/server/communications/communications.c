/* 
 * communications.c - CS50 'communications' module
 *
 * see communications.h for more information.
 *
 * Elisabeth Baumann November 2023
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "communications.h"
#include "hashtable.h"
#include "message.h"
#include "mem.h"
#include "server_structs.h"
#include "serverlogging.h"
#include "server_structs.c"
#include "gamestate.h"
#include "goldPiles.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/

/**************** global types ****************/

/**************** global functions ****************/
/* that is, visible outside this file */
/* see communications.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static bool handleMessage(void * arg, const addr_t from, const char* message);
static int parseMessage(const char * message);
static char parseKey(const char * message);
static bool parsePlayer(const char * message, char name[MaxNameLength+1]);
static bool sendMessage(const addr_t to, gamestate_t* game, char * message, int type);
static char findLetter(hashtable_t * players, const char * address);
static char * formatGridMessage(int width, int height);
static char* formatGoldMessage(int nugs, int purse, int remainder);
static char* convertNum(int num);
static void messagePlayersGold(void * arg, const char * key, void * item);
static char * formatDisplayMessage(int height, int width, char ** grid);
static void messagePlayersDisplay(void * arg, const char * key, void * item);


/**************** communications_listen() ****************/
/* see communications.h for description */
bool communications_listen(gamestate_t* game) {
  logMessage("Listening...");
  // Loop waiting for messages
  bool ok = message_loop(game, 0, NULL, NULL, handleMessage);

  // If there was an error, return 0
  if (!ok) {
    return(0);
  }
  return(1);
}

/**************** communications_sendMessage() ****************/
/* see communications.h for description */
static bool sendMessage(const addr_t to, gamestate_t* game, char * message, int type) {
  if (type == 1) { // one recipient
    message_send(to, message);
    logMessage("Message sent.");
  } else if (type == 2) { // all recipients
    // Can be one of two message types
    if (!strcmp(message, "GOLD ")) {
      hashtable_t * players = game->players;
      hashtable_iterate(players, game, messagePlayersGold);
      logMessage("Players updated with GOLD.");

    } else if (!strcmp(message, "DISPLAY ")) {
      logMessage("Preparing to send display to all players.");
      hashtable_t* players = game->players;
      hashtable_iterate(players, game, messagePlayersDisplay);
      logMessage("Players updated with DISPLAY.");
    }

  } else {
    return(0); // error
  }
  return(1);
}


static bool handleMessage(void * arg, const addr_t from, const char* message) {
  gamestate_t * game = arg;
  addr_t from_copy = from; // to avoid const warnings

  // Parse message and act accordingly
  int type = parseMessage(message);
  if (type == 0) {
    logError("Message is of invalid type.");
  } else if (type == 1) { // KEY
    char k = parseKey(message);
    
    // If quit
    if (k == 'Q') {
      // Check if it's the spectator
      int isSpectator = 0;
      spectator_t * tempSpec = game->activeSpectator;
      if (tempSpec != NULL) {
        addr_t * temp_addr = tempSpec->endpoint;
        if (!strcmp(message_stringAddr(from), message_stringAddr(*temp_addr))) {
          isSpectator = 1;
        }
      }

      // If it is the spectator
      if (isSpectator) {
        sendMessage(from, game, "QUIT you stopped spectating the game\0", 1);
        tempSpec->endpoint = NULL;
      } else {
        // Find letter corresponding to address
        char letter = findLetter(game->players, message_stringAddr(from)); // create function to find letter
        char stringLet[2] = "\0";
        stringLet[0] = letter;
        player_t * player = hashtable_find(game->players, stringLet);

        // Remove player
        if (removePlayer(player)) { // if successful
          sendMessage(from, game, "QUIT you have left the game\0", 1);
          logMessage("Player removed.");
        } else {
          logError("Could not remove player.");
        }
      }      
    } else { // if the key is anything other than Q, it's a move
      // Find the letter corresponding to this address
      char letter = findLetter(game->players, message_stringAddr(from));
      char letString[2] = "\0";
      letString[0] = letter;

      // Update the player's position
      updatePlayerPosition(game, game->players, letString, k);

      // Message spectator
      spectator_t * spec = game->activeSpectator;
      if (spec != NULL) {
        addr_t * temp_addr = spec->endpoint;
        char * display_message = formatDisplayMessage(game->gridHeight, game->gridWidth, game->grid);
        sendMessage(*temp_addr, game, display_message, 1);
        char * gold_message = formatGoldMessage(0, 0, game->remainingGold);
        sendMessage(*temp_addr, game, gold_message, 1);
      }
      
      // Message active players
      sendMessage(from, game, "DISPLAY ", 2);
      sendMessage(from, game, "GOLD ", 2);
    }
  } else if (type == 2) { // PLAYER
    char name[MaxNameLength+1]; // room for the max name length and a null

    // Check for name
    if (!parsePlayer(message, name)) {
      sendMessage(from, game, "QUIT player has no name\0", 1);
      logMessage("Player had no name, did not add.");
    } else {
      // Create player
      if (addPlayer(game, name, &from_copy)) { // if successfully added
        // Format message
          // Find the assigned letter & turn it into a string
        char letter = findLetter(game->players, message_stringAddr(from)); // create function to find letter
        char * letString = mem_malloc(2*sizeof(char));
        letString[0] = letter;
        letString[1] = '\0';
          // Create message string (will be 5 chars long for OK, space, letter, and null)
        char message[5];
        strcpy(message, "OK ");
        strcat(message, letString);

        // Send OK message
        sendMessage(from, game, message, 1);

        // Send GRID
        char * grid_message = formatGridMessage(game->gridWidth, game->gridHeight);
        sendMessage(from, game, grid_message, 1);

        // Message spectator
        spectator_t * spec = game->activeSpectator;
        if (spec != NULL) {
          addr_t * temp_addr = spec->endpoint;
          char * display_message = formatDisplayMessage(game->gridHeight, game->gridWidth, game->grid);
          sendMessage(*temp_addr, game, display_message, 1);
          char * gold_message = formatGoldMessage(0, 0, game->remainingGold);
          sendMessage(*temp_addr, game, gold_message, 1);
        }
        
        // Message all active players
        sendMessage(from, game, "DISPLAY ", 2);
        sendMessage(from, game, "GOLD ", 2);

      } else { // if unsuccessful
        sendMessage(from, game, "QUIT max players has been reached\0", 1);
        logMessage("Max players has been reached, player not added.");
      }
    }
  } else if (type == 3) { // SPECTATE
    // Save previous spectator
    logMessage("New spectator request.");
    spectator_t * temp_spectator = game->activeSpectator;
    logMessage("Previous spectator saved.");

    // Call addSpectator
    if (addSpectator(game, &from_copy)) {
      logMessage("Spectator added.");
      // Check if there was an existing spectator
      if (temp_spectator != NULL) { // if there was, send QUIT
        logMessage("Disconnecting previous spectator.");
        addr_t * temp_addr = temp_spectator->endpoint;
        sendMessage(*temp_addr, game, "QUIT replaced by other spectator\0", 1);
      }

      // Send GRID
      logMessage("About to send GRID to spectator.");
      char * grid_message = formatGridMessage(game->gridWidth, game->gridHeight);
      sendMessage(from, game, grid_message, 1);
      logMessage("Grid sent.");

      // Send DISPLAY to spectator
      logMessage("Preparing to send DISPLAY to spectator.");
      char * display_message = formatDisplayMessage(game->gridHeight, game->gridWidth, game->grid);
      logMessage("DISPLAY formatted.");
      sendMessage(from, game, display_message, 1);
      logMessage("DISPLAY sent.");

      // Send Gold to spectator
      logMessage("Preparing to send GOLD to spectator.");
      char * gold_message = formatGoldMessage(0, 0, game->remainingGold);
      logMessage("GOLD formatted.");
      sendMessage(from, game, gold_message, 1);
      logMessage("GOLD sent.");

    } else {
      // TO DO log error
      sendMessage(from, game, "QUIT unable to add spectator\0", 1);
      logMessage("Unable to add spectator.");
    }
  }
  
  // Check if the game is over
  if (game->remainingGold <= 0) {
    return(true); // GAME OVER, stop looping
  } else {
    return(false);  // keep listening
  }
}

static int parseMessage(const char * message) {
    // Variables
    char temp[10];
    int i = 0;

    // Loop through letters
    while (message[i] != ' ' && message[i] != '\0' && i < 9) {
        temp[i] = message[i];
        i++;
    }
    temp[i] = '\0';

    if (!strcmp(temp, "KEY")) {
        return(1);
    } else if (!strcmp(temp, "PLAY")) {
        return(2);
    } else if (!strcmp(temp, "SPECTATE")) {
        return(3);
    }
    return(0);
}

static char parseKey(const char * message) {
  // Variables
  char temp[6]; // message should have six characters (KEY, space, the keystroke, and the null)

  // Copy message into temp
  strcpy(temp, message);

  char k = temp[4];
  return(k);
}

static bool parsePlayer(const char * message, char name[MaxNameLength+1]) {
  // Variables
  int size = MaxNameLength+6;
  int length = strlen(message);
  char temp[size];

  for (int i=0; i<size && i<length; i++) {
    temp[i] = message[i];
  }

  // Starting after "PLAY "
  for (int i=5; i<size; i++) {
    name[i-5] = temp[i];
  }

  // Check if name is empty
  if (strlen(message) <= 6) {
    return(0);
  }

  return(1);
}

static char findLetter(hashtable_t * players, const char * address) {
  // Use hashtable_find to check if the address of a given letter matches mine
  char letter;

  for (letter='A'; letter < 91; letter++) {
    char * letString = mem_malloc(2*sizeof(char));
    letString[0] = letter;
    letString[1] = '\0';
    player_t * tempPlayer = hashtable_find(players, letString);
    addr_t * tempAddr = tempPlayer->endpoint;
    if (!strcmp(address, message_stringAddr(*tempAddr))) { // if the address is the same, stop loop
      break;
    }
  }

  if (letter < 91) {
    return(letter);
  } else {
    return('?');
  }
}

static char * formatGridMessage(int width, int height) {
  // Convert width and height to strings
  char * widthString = convertNum(width);
  char * heightString = convertNum(height);
  logMessage("Height and width converted.");

  // Create message string of the right size
  int totSize = strlen(widthString)+strlen(heightString)+7; // includes space for width, height, GRID, two spaces, and null
  char * message = mem_malloc(totSize*sizeof(char));
  strcpy(message, "GRID ");
  strcat(message, heightString);
  strcat(message, " ");
  strcat(message, widthString);
  logMessage("Message formatted.");

  return(message);
}

static char* formatGoldMessage(int nugs, int purse, int remainder) {
  logMessage("Preparing to convert numbers into strings.");
  // Convert numbers to strings
  char * nugsString = convertNum(nugs);
  logMessage("Converted nugs.");
  char * purseString = convertNum(purse);
  logMessage("Converted purse.");
  char * remainderString = convertNum(remainder);
  logMessage("Converted remaining gold.");

  // Create message string of the right size
  int totSize = strlen(nugsString)+strlen(purseString)+strlen(remainderString)+9; // includes space for number strings, GOLD, three spaces, and null
  char * message = mem_malloc(totSize*sizeof(char));
  logMessage("Allocated space for gold message.");
  strcpy(message, "GOLD ");
  strcat(message, nugsString);
  strcat(message, " ");
  strcat(message, purseString);
  strcat(message, " ");
  strcat(message, remainderString);
  strcat(message, "\n");
  logMessage("Message assembled.");

  return(message);
}

static char* convertNum(int num) {
  // Find size of number
  int copy_num = num;
  int numSize = 1;

  // include case for 0
  if (num == 0) {
    numSize++;
  }

  while (copy_num > 0) {
    copy_num = copy_num/10;
    numSize++;
  }

  // Convert width to a string
  char * numString = mem_malloc(numSize*sizeof(char));
  sprintf(numString, "%d", num);

  return(numString);  
}

static char * formatDisplayMessage(int height, int width, char ** grid) {
  // Initiate a message string
  char * message = mem_malloc(height*(width+1)+9); // space for all chars in grid, DISPLAY, an endline, and a terminating null on each line

  // Copy in "DISPLAY\n"
  strcpy(message, "DISPLAY\n");

  // Loop through each row and strcat the row onto the message
  for (int i=0; i<height; i++) {
    strcat(message, grid[i]);
    strcat(message, "\n");
  }

  return(message);
}

static void messagePlayersGold(void * arg, const char * key, void * item) {
  // Set an int equal to arg
  gamestate_t * game = (gamestate_t *)arg;
  int remainder = game->remainingGold;

  // Find player structure
  player_t * player = item;

  if (player->activeStatus) { // if they're active
    // Combine message
    char * message = formatGoldMessage(0, player->purse, remainder);

    // Send message
    addr_t * tempAddr = player->endpoint;
    message_send(*tempAddr, message);
  }
}

static void messagePlayersDisplay(void * arg, const char * key, void * item) {
  // Find player and game structures
  gamestate_t * game = (gamestate_t *)arg;
  player_t * player = item;
  logMessage("Structures found successfully.");

  if (player->activeStatus) { // if they're active
    logMessage("Active player found.");
    // Combine message
    char * message = formatDisplayMessage(game->gridHeight, game->gridWidth, player->visibleGrid);
    logMessage("Visibility grid formatted.");

    // Send message
    addr_t * tempAddr = player->endpoint;
    message_send(*tempAddr, message);
  }

}