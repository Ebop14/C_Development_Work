#ifndef CLIENT_H
#define CLIENT_H

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../support/message.h"

/******** global types ********/
struct gameState;
//static gameState;

/******** handle_input ********/
/* creates a new, empty index
 *
 * Caller provides:
 *  number of slots to be used in the index
 * 
 * We gurantee:
 * a index is made with hashtable of that many sized slots
 *
 * We return:
 *  pointer to the new index or NULL if error
 *
 */
static bool handle_input(void* arg);

/**************** functions ****************/

/**************** handle_input ****************/
/* Handle the keystrokes the user inputs
 *
 * We return:
 *   a boolean value, false if there are inputted characters
 * We guarantee:
 *   only one character is sent to the server at a time,
 *  in the form of "KEY c" where c is the character
 *  ensures the message_loop continues as long as a character is inputted to the server
 *  
 */
static bool handle_input(void* arg);
/**************** handle_message ****************/
/* Handle the messages received from the server
 *
 * We return:
 *   a boolean value, false ensures the message_loop continues to run
 * We guarantee:
 *   the client will print to its display all messages that start with the words
 *   OK, GRID, GOLD, DISPLAY, QUIT, ERROR
 *   anything else and the function returns true so that the message_loop stops running
 *   QUIT also returns true so that message_loop stops running
 */
static bool handle_message(void* arg, const addr_t from, const char* message);
/**************** handle_gridmessage ****************/
/* Handles the messages from server that start with "GRID"
 *
 * We return:
 *   an error if window not big enough
 * We guarantee:
 *   for the grid size the server sends to the client
 *   we check if the grid size fits the given window size
 *   if not, we tell client to resize the screen 
 */
void handle_gridmessage(const char* message);
/**************** handle_display ****************/
/* Handles the messages from server that start with "DISPLAY"
 *
 * We return:
 *   nothing
 * We guarantee:
 *    for the message that begins with "DISPLAY\n"
 *    we print the map that the server inputs to the window of the client
 *   
 */
void handle_display(const char* message);
/**************** handle_gold ****************/
/* Handles the messages from server that start with "GOLD "
 *
 * We return:
 *   nothing
 * We guarantee:
 *    for the message that begins with "GOLD n p r"
 *    we print the gold left, gold in purse and the gold received(if it has been received) to the window of the client
 *   
 */
void handle_gold(const char* message);
/**************** handle_error ****************/
/* Handles the messages from server that start with "ERROR"
 *
 * We return:
 *   nothing
 * We guarantee:
 *    for the message that begins with "ERROR"
 *    we print the error message to the window of the client
 *   
 */
void handle_error(const char* message);
/**************** handle_quit ****************/
/* Handles the messages from server that start with "QUIT"
 *
 * We return:
 *   nothing
 * We guarantee:
 *    for the message that begins with "QUIT"
 *    shuts down the window display
 *    we print the quit message to the stdin
 *   
 */
void handle_quit(const char* message);

#endif // CLIENT_H