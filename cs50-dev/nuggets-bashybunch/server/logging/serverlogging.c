/*
serverlogging.c
Implementation of the server's logging module.

See serverlogging.h for detailed description.

Lucas Mazzucchelli - Dartmouth CS50 Nuggets, Fall 2023
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "serverlogging.h"


// Function prototypes
void getFormattedTimestamp(char* buffer, size_t bufferSize);


/*
See serverlogging.h for description.
*/
void serverlogInit(FILE* fp) {
    log_init(fp);
}


/*
See serverlogging.h for description.
*/
void logMessage(char* message) {
    // Prefix message with timestamp
    char buffer[256];
    char timestamp[20];
    getFormattedTimestamp(timestamp, sizeof(timestamp));
    snprintf(buffer, sizeof(buffer), "%s - %s", timestamp, message);

    // Log to file
    log_v(buffer);
}


/*
See serverlogging.h for description.
*/
void logFormattedString(char* format, char* string) {
    // Prefix format with timestamp
    char buffer[256];
    char timestamp[20];
    getFormattedTimestamp(timestamp, sizeof(timestamp));
    snprintf(buffer, sizeof(buffer), "%s - %s", timestamp, format);

    // Log to file
    log_s(buffer, string);
}


/*
See serverlogging.h for description.
*/
void logFormattedChar(char* format, char c) {
    // Prefix format with timestamp
    char buffer[256];
    char timestamp[20];
    getFormattedTimestamp(timestamp, sizeof(timestamp));
    snprintf(buffer, sizeof(buffer), "%s - %s", timestamp, format);

    // Log to file
    log_c(buffer, c);
}


/*
See serverlogging.h for description.
*/
void logError(const char* message) {
    // Build error message
    char errorMsg[256] = "ERROR: ";
    strncat(errorMsg, message, sizeof(errorMsg) - strlen(errorMsg) - 1);

    // Log error message in log file
    logMessage(errorMsg);

    // Print error message to stderr with timestamp
    char buffer[512];
    char timestamp[20];
    getFormattedTimestamp(timestamp, sizeof(timestamp));
    snprintf(buffer, sizeof(buffer), "%s - %s", timestamp, errorMsg);
    
    // Use a string literal for the format
    fprintf(stderr, "%s", buffer);
}


/*
Helper function that gets current timestamp and formats.
*/
void getFormattedTimestamp(char* buffer, size_t bufferSize) {
    time_t now = time(NULL);
    struct tm timestamp = *localtime(&now);
    strftime(buffer, bufferSize, "%Y-%m-%d %H:%M:%S", &timestamp);
}


/*
See serverlogging.h for description.
*/
void serverlogDone() {
    log_done();
}
