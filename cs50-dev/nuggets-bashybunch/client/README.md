### README for Client

## Description
The client allows a human to join the game as an interactive player or as a spectator.The client starts from the commandline and is initialized with a server address and portnumber. A third argument determines the player and their name, no third argument deems the client as spectator. 


## Function Prototypes

`static bool handle_input(void* arg);
static bool handle_message(void* arg, const addr_t from, const char* message);
void handle_gridmessage(const char* message);
void handle_ok(const char* message);
void handle_display(const char* message);
void handle_gold(const char* message);
void handle_error(const char* message);
void handle_quit(const char* message);
` 

## Control flow

The client is implemented in client.c with nine functions.

#main
Parses arguments to ensure that there are 2 or 3 arguments and uses arguments to create server address to send and receive messages from. Sends the player type to server, either spectator or player, and initializes the message_loop function. 

#handle_input
ensures server address exists and reads each keystroke one character at a time. It sends each individual keystroke to the server in the form of "KEY character". The message_loop continues until there is no more input.

#handle_message
Calls one of any 6 functions that can handle the messages the server will send to the client. Scans the message to ensure it is in one of any 6 forms. If not, message_loop stops, or if message is QUIT.

#handle_ok
Sets the player character according to the server message.

#handle_gridmessage
Ensures that the client window is big enough for the map grid.

#handle_display
prints map sent after "DISPLAY" message to the client window.

#handle_gold
For player, prints to the display the updated gold remaining and gold in purse for the client, and the gold received (optional) if just received. For spectator, only prints the nuggets that remain.

#handle_error
prints the error message to the display

#handle_quit
exits the display window and prints the quit message to the stdin.

## libraries:

libraries used are support.a and ncurses.

## Error handling

Command line parameters are checked to ensure there are a proper number of arguments and the arguments can be handled by the functions that call them. The server is checked and ensures that its address exists. If a message is returned from the server that is null, an error will occur because all messages must be tagged with the format OK, GRID, GOLD, DISPLAY, ERROR, or QUIT.