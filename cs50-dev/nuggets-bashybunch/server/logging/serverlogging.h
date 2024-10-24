/*
serverlogging.h
Header file for the server's logging module.

Utilizes the logging module from the support directory.
Messages are logged to a specified file, and errors are also printed to stderr.
Messages are also prefixed with a timestamp.

Lucas Mazzucchelli - Dartmouth CS50 Nuggets, Fall 2023
*/


#ifndef _SERVER_LOGGING_H_
#define _SERVER_LOGGING_H_


#include <stdio.h>
#include <stdlib.h>
#include "../../support/log.h"


/*
Initialization function to begin logging, given a pointer to a file open for writing.
*/
void serverlogInit(FILE* fp);


/*
Function to log a general message with a timestamp.
*/
void logMessage(char* message);


/*
Function to log a string using a specified format string.
*/
void logFormattedString(char* format, char* string);


/*
Function to log a character using a specified format string.
*/
void logFormattedChar(char* format, char c);


/*
Function to log an error message with a timestamp and print it to stderr.
*/
void logError(const char* message);


/*
Function to pause or end logging. Call serverlogInit() to resume logging.
*/
void serverlogDone();


#endif // _SERVER_LOGGING_H_
