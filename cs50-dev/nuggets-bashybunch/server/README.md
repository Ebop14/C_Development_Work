### README for Server

## Description
The server hosts the nuggets game, and changes the "true" nuggets game whenever a client inputs a relevant keystroke. The server is initialized with a path to a map, which is parsed and initialized by gamestate. After parsing the arguments and initializing the server, the client can connect to the server via the specified port. 


## Function Prototypes

```c
int validateArguments(int argc, char* argv[]);
```

```c
int main(int argc, char* argv[])
```

## Control flow

The client is implemented in client.c with two functions.


#main
The main function calls validateArguments to validate the commandline arguments. It then generates a random seed if the user did not specify a map to start from. If the player did provide a map, the main function checks the map file's readability and creates the pointer to the log file. It then initializes the logging module, the communications module, the gamestate module, and listens for communication from the client. After termination of the game, the game deletes all relevant memory and closes relevant file pointers.

#validateArguments
ValidateArguments first validates the argument count. It then checks the map extension, and checks to see if it's a .txt file. It then validates the random seed provided (if relevant).

## libraries:

server uses the libcs50 library, the common library, the communications module, the loggging module, and the gamestate module

## Error handling

Command line parameters are checked to ensure there are a proper number of arguments and the arguments can be handled by the functions that call them. The mapfile is checked to see if it exists, is a .txt file, and is readable. If relevant, the random seed's initial value is checked as well. The server also checks whether the log file can be written to as well. It also checks whether the communications module can be initialized. Finally, the server checks whether the game can be initialized. 