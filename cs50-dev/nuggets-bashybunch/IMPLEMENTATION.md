# CS50 Nuggets
## Implementation Spec
### Team bashy bunch, Fall, 2023

## Plan for division of labor

> Ethan: Gamestate initialization, movement, and visibility, player data structure, Implementation Spec

> Elisabeth: Server module

> Jillian: Client module

> Lucas: Logging Module, how the gamestate handles the players

# Player Module
```c
struct player {
    char* name;
    char letter;
    position_t* pos;
    int purse;
    char* endpoint;
    char** visibleGrid;
    bool activeStatus;
};
```
> Player: A data structure that contains the information for a given player. This is a representation of a given client's game.
* name: The name that the player enters when they first connect to the server
* letter: The letter that the server assigns to the player when they first join. 
* pos: a data structure representing the x and y coordinate for the player
* purse: an int representing how much gold the player has collected
* endpoint: the port of the client
* visibileGrid: A 2D array of chars that represents the grid that can be seen by the player given their current position
* activeStatus: a boolean that indicates whether a player is active (and whether we should accept messages from them)


# GameState Module
The Gamestate module handles all information related to the game, such as players, the map, gold piles, etc.

### Data structures

> gamestate
> gamestate is a data structure that holds all of the game's information. It's what gets updated when anything happens between the players and the server. 
```c
 struct gamestate {
    char** grid; 
    int remainingGold; 
    int numPiles; 
    goldPiles_t* piles; 
    int gridWidth; 
    int gridHeight; 
    hashtable_t* players;
    spectator_t* activeSpectator;
};
```
* grid: A 2D array that contains every character in the map
* remainingGold: An int that represents the total gold to be collected, dispersed over numPiles
* numPiles: An int that represents how many piles of gold need to be collected
* piles: A goldPiles data structure keyed using coordinates (x, y). The first key is x, which maps to a counters data structure. The second key is y, which maps to the amount of gold at the coordinate (x, y)
* gridWidth: The width of the map, in number of chars
* gridHeight: The height of the map, in number of chars
* players: A hashtable that maps a player's assigned letter to their player data structure
* activeSpectator: The current spectator

### Definition of function prototypes

A helper function that deletes items in a gold pile. 

```c
void pile_item_delete(void* item);
```

A function to delete all aspects of the game, freeing memory where appropriate
```c
void gamestateDelete(gamestate_t* game);
```

A function to initialize the gamestate data structure from the file pointer
```c
gamestate_t* initializeGame(FILE* fp);
```

A function that takes a gamestate, a player hashtable, a player, and a movement message and moves the player, handling everything that goes with it. This includes, but is not limited to, visibility, collisions, and collecting gold. 
```c
void updatePlayerPosition(gamestate_t* game, hashtable_t* playerTable, char* player, char message);
```

A function that takes in a pointer to a given player and a pointer to the game and redraws their visibility grid as appropriate

```c
void redraw(player_t** player_pointer, gamestate_t** game_pointer);
```

A function that, given a player and a gamestate, checks whether the player has moved onto gold

```c
void checkForGold(player_t** player, gamestate_t** game);
```

A function that, given a pointer to a player, a pointer to a game, and x and y coordinates to check, checks whether the player can see that point. 

```c
bool checkPoint(player_t** player_ptr, gamestate_t** game_ptr, int xCheck, int yCheck);
```

A function that adds a player to a game with their entered name, connecting the client to the server via their endpoint.

```c
bool addPlayer(gamestate_t* game, const char* name, const char* endpoint);
```

A function that removes a player from the game 

```c
bool removePlayer(player_t* player);
```

A function that adds a spectator to the game, connecting the client to the server via the endpoint. 
```c
bool addSpectator(gamestate_t* game, char* newEndpoint);
```


### Detailed pseudo code

#### `gamestateDelete`:

	validate parameters
    log the deletion
    free all apsects of the gamestate

---

#### `initializeGame`:

	log the initialization
    read the map from the file
    for every column in the map:
        for every row in the map:
            if the character is a \n:
                start a new line
            if the character is a \0:
                stop copying
            otherwise:
                copy the character into the map

    create the gold piles
    initialize every pile to a random amount of gold and random coordinates

---

#### `updatePlayerPosition`:

	get the player we want from playerTable
    for the correct keystroke:
        if the new position is a wall, corner, or empty space:
            return
        else:
            move the player
            call CheckForGold
            iterate over the players, calling checkCoords on each one
            call redraw on the player
    
---

#### `redraw`:

	iterate through every player, and draw their letter onto the map

    if the character the player is on contains gold, set its value to 0

    for every x value in the map:
        for every y value in the map:  
            if the character is a *:
                if the gold at that position has value 0, continue
                otherwise, draw it
            if checkPoint returns false for x, y:
                draw the point as a ' '
            otherwise:
                draw the point
            
---

#### `iteratePlayers`:

	get the x and y position from the player
    get the player's letter
    draw the player's letter onto the map
            
---

#### `checkCoords`:

	compare our player's coordinates to a given other player's coordinates

    if they match:
        swap the two's coordinates
            
---

#### `checkPoint`:

	find the change in x and y from our endpoint and our player

    calculate the slope off of that

    while we haven't reached our endpoint:
        set newX to our origin
        find newY by multiplying the origin by the slope

        if the point directly falls onto a character:
            check if that character should be visible
            if it's not, then return false
        if the point falls in between two characters:
            for the characters to the left and right of the line:
                if the character shouldn't be visible:
                    return false
                otherwise, continue
        
        if origin is less than 0, iterate it by -1. 
        if origin is greater than 0, iterate it by 1

            
---

#### `checkforGold`:

	if the player's position contains gold:
        add it to their purse
        decrement the remainingGold from the gamestate
            
---


#### `intToString`:
    check if the entered number is 0
    if num is not zero, divide by 10 until num reaches 0
        count each loop, which equals the number of digits
    use the number of digits to create a string of the appropriate size
    copy int into the new string

	if the player's position contains gold:
        add it to their purse
        decrement the remainingGold from the gamestate
            
---


#### `addPlayer`:
    log the player addition

    check if the maximum number of players has been exceeded

    truncate the player's name

    generate a letter for the player

    generate their starting position randomly

    assign their endpoing via their port

    add the player to the hashtable, with key letter and value player

    call redraw for the new player

    log the addition
            
---

#### `removePlayer`:
    log the removal request
    check if the player is NULL

    if the player is active:
        set activeStatus to false
    else:
        log an error
        
---

#### `addSpectator`:
    log the request to add a spectator

    create a new spectator object

    if there is an active spectator, remove them

    create the endpoint for the new spectator

    copy the port onto the new spectator's endpoint
    
    set the activespectator to the new spectator

    log the success
        
---

#### `freePlayer`:
    free the player's:
        name
        endpoint
        position data structure
        visible grid
    free the player pointer
        
---

#### `freeSpectator`:
    free the spectator's endpoint
    free the spectator pointer
---



## goldPiles Module

goldPiles
goldPiles is a wrapper for hashtable that always maps to a counters object. It's meant to hold the coordinates of every gold pile and the amount of gold at the coordinate. 
The x coordinate keys from the hashtable to a counters data structure.
The y coordinate keys from the counters structure to an int that represents the amount of gold.
So, the structure goes as follows: x -> y -> amount of gold


### Data structures

```c
typedef hashtable_t goldPiles_t;
```
* goldPiles is a wrapper for hashtable

### Definition of function prototypes

A function that initializes the goldPiles data structure with a given number of slots
```c
goldPiles_t* piles_new(const int num_slots);
```

A function that sets a gold pile's gold amount given x and y coordinates
```c
bool piles_set(goldPiles_t* piles, char* x, int y, int gold);
```

A function that returns a counter to a gold pile given an input x
```c
void* piles_find(goldPiles_t* pile, const char* x);
```

Sets a pile's amount of gold at coordinates x,y to a given amount
```c
bool piles_add(goldPiles_t* pile, const char* x, const int y, int amount);
```

Calls itemfunc on every item in the goldPiles data structure
```c
void piles_iterate(goldPiles_t* piles, void* arg, void (*itemfunc)(void* arg, const char* key, void* item));
```

Deletes the pile data structure
```c
void piles_delete(goldPiles_t* index);
```

### Detailed pseudo code
* All of the above functions are wrappers for hashtable
---

# Logging module
The logging module handles all of the server and client logging

### Data structures
* None

### Definition of function prototypes
Initializes the logging function to a file, given a file pointer
```c
void serverlogInit(FILE* fp);
```

Logs a general message with a timestamp
```c
void logMessage(char* message);
```

Logs a formatted string
```c
void logFormattedString(char* format, char* string);
```

Logs a specific character using a specified format
```c
void logFormattedChar(char* format, char c);
```

Logs a given error message to stderr with a timestamp
```c
void logError(const char* message);
```

Stops all logging. Logging can be resumed by calling serverlogInit again
```c
void serverlogDone();
```

### Detailed pseudo code


#### `serverlogInit`:
    initialize the server logger with the given filepointer
        
---

#### `logMessage`:
    create a buffer for the messsage
    get the timestamp for the message
    copy the message and the timestamp onto the buffer
    log the buffer to the file        
---

#### `logFormattedString`:
    create a buffer for the message
    get the timestamp for the message
    copy the timestamp and the format onto the buffer
    log the buffer and the message to the log
---

#### `logFormattedChar`:
    create a buffer for the message
    get the timestamp for the message
    copy the timestamp and the format onto the buffer
    log the buffer and the cahracter to the file
---

#### `logError`:
    create an error message
    create a buffer for the error message
    copy the timestamp and the error message onto the buffer
    print the buffer to stderr
---

## Communications Module
The communications module handles all communication between the various clients and the server

### Data Structures
No data structures were created for the communications module

### Function Prototypes

Waits until a player sends a message to the client, and calls handleMessage when one does.
```c
bool communications_listen(gamestate_t* game)
```

Sends a message to a given number of players, depending on the type of message to be sent
```c
static bool sendMessage(const addr_t to, gamestate_t* game, char * message, int type);
```

Parses a message from a given address and affects the game accordingly
```c
static bool handleMessage(void * arg, const addr_t from, const char* message);
```

Loops through the letters and parses a general message
```c
static int parseMessage(const char * message);
```

Parses a keystroke message
```c
static char parseKey(const char * message);
```

Takes a player's name input and checks if they had input a name
```c
static bool parsePlayer(const char * message, char name[MaxNameLength+1]);
```

Given a hashtable of players and a given player's IP address, find the letter that corresponds to the address
```c
static char findLetter(hashtable_t * players, const char * address);
```

Formats the grid as one long string given a width and height
```c
static char * formatGridMessage(int width, int height);
```
Formats a gold collection message for when a player collects gold
```c
static char* formatGoldMessage(int nugs, int purse, int remainder)
```

Converts an int into a string
```c
static char* convertNum(int num);
```

hashtable_iterate helper function that sends a gold collection message to all players 
```c
static void messagePlayersGold(void * arg, const char * key, void * item);
```
Formats the message to send to all players as a grid display message
```c
static char * formatDisplayMessage(int height, int width, char ** grid);
```

hashtable_iterate helper function that broadcasts the grid to all players
```c
static void messagePlayersDisplay(void * arg, const char * key, void * item);
```

### Detailed Pseudocode

#### `communications_listen`:
    start a message loop listening for messages
---

#### `sendMessage`:
    if the message should be sent to one recipient:
        call message_send to the one player
    if the message should be sent to multiple recipients:
        if the message type is type gold:
            call messagePlayersGold for every player in hashtable_iterate
        if the message type is type display:
            call messagePlayersDisplay for every player in hashtable_iterate

---

#### `handleMessage`:
    parse the message
    if the message is an error:
        log it as an error
    if the message is a key:
        call parseKey
        
        if the key is type quit:
            call findLetter to find the letter corresponding to the address of the player
            remove the player from the game 
        if the key is not type quit:
            update the player's position
            send the updated grid to every player
            send the updated gold to every player
    if the message is to spectate:
        create a spectator
        if there is an existing spectator:
            replace the old spectator with a new one
        send the grid to the spectator
        send the display message to the spectator
        send the gold message to the spectator
    
    if all the gold has been collected:
        return
---

#### `parseMessage`:
    copy the message into a buffer
    if the message is key:
        return 1
    if the message is play:
        return 2
    if the message is spectate:
        return 3
    return 0
---

#### `parseKey`:
    allocate a buffer of size 6
    copy the message onto the buffer
    get the fifth character of the buffer, because the key message is always formatted as KEY (space character) char

---

#### `parsePlayer`:
    allocate a buffer
    copy the message onto the buffer
    starting from the fifth index, parse the name
        we start from the fifth index because the player message is always PLAY (space character)
    if the name is empty, return an error
---

#### `findLetter`:
    for every letter:
        get a player data structure from the current letter
        if the IP address of the player extracted matches the IP address passed through as a parameter:
            break
    if the letter is in the alphabet, return it
    if the letter isn't return ?
---

#### `formatGridMesssage`:
    convert the width and the heights of the grid to strings

    create a message of size width and height

    return the message out with format:
        GRID heightString widthSTring
---

#### `formatGoldMessage`:
   convert the nugget, the purse, and remaining gold into strings
   create a message of format:
        GOLD nuggetString purseString remainingString
    return the message
---

#### `convertNum`:
    check if the entered number is 0
        if num is not zero, divide by 10 until num reaches 0
            count each loop, which equals the number of digits
        use the number of digits to create a string of the appropriate size
        copy int into the new string

        if the player's position contains gold:
            add it to their purse
            decrement the remainingGold from the gamestate
---

#### `formatDisplayMessage`:
    allocate a message string
    add DISPLAY to the start of the message
    for each row:
        concatenate the row onto the message string
    return the message
---

#### `messagePlayersGold`:
    cast the item as a player
    if the player is active:
        get the player's address
        call formatGoldMessage
        send the message to the player
---

#### `messagePlayersDisplay`:
    cast the arg as a gamestate
    cast the item as a player
    if the player is active:
        get the player's address
        call formatDisplayMessage
        send the message to the player
---

## Server module
This module validates the arguments from stdin and runs the server

### Data Structures 
Server Module did not create any new data structures

### Function Prototypes
Takes the arguments from stdin and validates them
```c
int validateArguments(int argc, char* argv[]);
```

### Detailed Pseudocode

#### `validateArguments`:
    validate the argument counts
    validate the map filepath by checking its .txt extension

    if a seed is given, validate it
---

## Client Module
This module handles the client side of the nuggets game.

### Data Structures
The client module doesn't create any new data structures, but it does use gameState

### Function Prototypes

```c
Creates a new, empty input from our argument
static bool handle_input(void* arg);
```

Handles the keystroke that the user inputs
```c
static bool handle_input(void* arg);
```

Handles the messages recieved from the server
```c
static bool handle_message(void* arg, const addr_t from, const char* message);
```

Handles the messages from the server of type GRID
```c
void handle_gridmessage(const char* message);
```

Handles the messages from the server of type DISPLAY
```c
void handle_display(const char* message);
```

Handles the messages from the server of type GOLD
```c
void handle_gold(const char* message);
```

Handles the messages from the server of type ERROR
```c
void handle_error(const char* message);
```

Handles the messages from the server of type QUIT
```c
void handle_quit(const char* message);
```

### Detailed Pseudocode

#### `main`:
    validate the arguments
    set up the message address
    if the mode is spectate:
        send a spectate message to the server
    else:
        send a play message to the server
    start a message loop to listen for messsages from the server or client
---

#### `handle_input`:
    validate the parameters

    get the keystroke from the message
    create a message to hold KEY and the keystroke
    send the message to the server
    refresh
---

#### `handle_message`:
    validate the parameters
    if the message is type OK:
        call handle_ok on the message
    if the message is type GRID:
        call handle_gridmessage on the message
    if the message is type GOLD:
        call handle_gold on the message
    if the message is type DISPLAY:
        call handle_display on the message
    if the message is type QUIT:
        call handle_quit on the message
    if the message is type ERROR:
        call handle_error on the message
    if the message is invalid:
        return
---

#### `handle_ok`:
    parse the message for the char
    set the gamestate's player's char to the message
---

#### `handle_gridmessage`:
    turn on color controls
    get the maximimum allowable rows and columns for the given window
    parse the message and get the number of grid rows and columns
    if the screen is too small to display the grid:
        ask the user to resize
---

#### `handle_display`:
    parse the display from the message
    print the display into to window
    refresh the screen
---

#### `handle_gold`:
    parse the message for the client's current active status, their purse, and how much gold they have yet to collect
    if the client is a player:
        print the player's character, their pruse, and how much gold is left in the grid
    if the client is a spectator:
        print the number of unclaimed nuggets in the grid
    refresh the window
---

#### `handle_error`:
    parse the error message
    print the error message
    refresh the window
---

#### `handle_quit`:
    parse the quitting message
    exit curses
    print the quitting message
---

## Testing plan

### unit testing

> How will you test each unit (module) before integrating them with a main program (client or server)?

### integration testing

> How will you test the complete main programs: the server, and for teams of 4, the client?

### system testing

> For teams of 4: How will you test your client and server together?

---

## Limitations

> Bulleted list of any limitations of your implementation.
> This section may not be relevant when you first write your Implementation Plan, but could be relevant after completing the implementation.
