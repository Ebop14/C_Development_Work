#include "client.h"

#define maxMessage 65507 // equivalent to message_MaxBytes

/******************** LOCAL STRUCT *************************/
typedef struct player{
  char character;
  int left;  //gold left in game
  int received; //gold player just received
  int purse; //gold in purse
} player_t;

/******************** GLOBAL STRUCT *************************/
static struct {
  addr_t server; // address of the server
  player_t player; //holds player info
  char name[maxMessage]; //name client inputs on command line
} gameState;

/**************** global variables ****************/
const int rowPosition = 0; //what row cursor moves to when mvprintw is called
const int colPosition = 50;  //what column cursor moves to when mvprintw is called
char* display;   //holds the 2-D string of the map to be displayed
char* quitMessage;  //holds any of the 4 given quit messages the server will send
char* errorMessage;  //stores the explanation fo error

/**************** file-local functions ****************/

static bool handle_input(void* arg);
static bool handle_message(void* arg, const addr_t from, const char* message);
void handle_gridmessage(const char* message);
void handle_ok(const char* message);
void handle_display(const char* message);
void handle_gold(const char* message);
void handle_error(const char* message);
void handle_quit(const char* message);

/***************** main *******************************/
int main (const int argc, char* argv[]){

  // initialize the message module (without logging)
  if (message_init(NULL) == 0) {
    return 2; // failure to initialize message module
  }
  
  // check arguments
  if (argc < 3 || argc > 4) {
    fprintf(stderr, "incorrect number of commandline args");
    message_done();
    return 2; // bad commandline
  }
  
  // commandline provides address for server
  const char* serverHost = argv[1];
  const char* serverPort = argv[2];
  
  if (!message_setAddr(serverHost, serverPort, &gameState.server)) {  // set the address based on given hostname and port
    fprintf(stderr, "can't form address from %s %s\n", serverHost, serverPort);
    message_done();
    return 4; // bad hostname/port
  }

// send mode of either play or spectate to the server
  if (argc < 4){
    gameState.name[0] = '\0';   //name is null when spectator
    message_send(gameState.server, "SPECTATE");
  }

  if (argc == 4){
    strcpy(gameState.name, argv[3]);
    char* play = (char *)malloc((strlen("PLAY ") + strlen(gameState.name) + 1) * sizeof(char));
    sprintf(play, "PLAY %s", gameState.name);
    message_send(gameState.server, play);
    free(play);
  }

  //valid input so initialize display
  initscr();
  cbreak();
  noecho();

  // Loop, waiting for input or for messages; provide callback functions.
  // We use the 'arg' parameter to carry a pointer to 'server'.
  message_loop(NULL, 0, NULL, handle_input, handle_message);

  // shut down the message module
  message_done();
  
  return 0; // status code depends on result of message_loop
  
}

/**************** handle_input ****************/
/* each keystroke is input; read each keystroke and send it to the client.
 * Return true if the message loop should exit, otherwise false.
 * i.e., return true if EOF was encountered on stdin, or fatal error.
 */

static bool
handle_input(void* arg)
{

if (&gameState.server == NULL) {
  fprintf(stderr, "handleKey called with serverAddress=NULL\n");
  return true; // returns true if message loop should exit
}

if (!message_isAddr(gameState.server)) {
  fprintf(stderr, "handleInput called without a correspondent.");
  return true;
}

  char ch = getch(); // will read each keystroke into here
  char message[strlen("KEY " + 2)]; // big enough to hold KEY and letter

  sprintf(message, "KEY %c", ch); // build message string
  message_send(gameState.server, message); // send it

  if (gameState.name[0] != '\0') { // check if player

    move(0, 50);
    clrtoeol();                   
  }

refresh();
// normal case: keep looping
return false;
}


/**************** handle_message ****************/
/* Datagram received; print it.
 * We ignore 'arg' here.
 * Return true if any fatal error.
 */
static bool handle_message(void* arg, const addr_t from, const char* message)
{
  if(message == NULL){ // check if message is null
    fprintf(stderr, "NULL message received");
    return true; 
  }

  if(strncmp(message, "OK ", 3) == 0){
    handle_ok(message);
    return false;
  }

  else if(strncmp(message, "GRID ", 5) == 0){
    // get number of rows in y and number of columns in x
    handle_gridmessage(message);
    return false;
    } 

  else if(strncmp(message, "GOLD ", 5) == 0){
    handle_gold(message);
    return false;
  }

  else if(strncmp(message, "DISPLAY", 7) == 0){
    handle_display(message);
    return false;
  }

  else if(strncmp(message, "QUIT ", 5) == 0){
    handle_quit(message);
    return true;  //stop message loop if quit message
  }

  else if(strncmp(message, "ERROR ", 6) == 0){
    handle_error(message);
    return false;
  }

  //incorrect inputs
  else {
    return true;
  }
  
}

void handle_ok(const char* message){
  //parse message to get the player letter
  sscanf(message, "OK %c", &gameState.player.character);
}

void handle_gridmessage(const char* message){
    //initialize color of the screen
    start_color(); // turn on color controls
    init_pair(1, COLOR_YELLOW, COLOR_BLACK); // define color 1
    attron(COLOR_PAIR(1)); // set the screen color using color 1

    int nrow; // windo row
    int ncol; //window column
    getmaxyx(stdscr, nrow, ncol);

    int gridRow; //grid rows
    int gridCol;  // grid columns

    //parse message and get the number of grid rows and columns
    sscanf(message, "GRID %d %d", &gridRow, &gridCol);

    //cannot make window and tell client to keep resizing until proper window size
    if (nrow + 1 <= gridRow || ncol + 1 <= gridCol){
      mvprintw(rowPosition, colPosition, "Must resize screen to fit %d rows and %d columns", gridRow + 1, gridCol + 1);
    }
  
}

void handle_display(const char* message){

  const char* screen = message + strlen("DISPLAY\n");

  mvprintw(1, 0, "%s", screen); // prints the grid onto the screen
  refresh();

  // sscanf(message, "DISPLAY\n%s", display);
  // mvprintw(1, 0, "%s", display);
  // refresh(); // redraw the screen
}

void handle_gold(const char* message){
  //check if the status is spectator or player
  sscanf(message, "GOLD %d %d %d", &gameState.player.left, &gameState.player.purse, &gameState.player.received);

  if (gameState.name[0] != '\0') {   //check if player
    mvprintw(0,0, "Player %c has %d nuggets (%d nuggets unclaimed).", gameState.player.character, gameState.player.purse, gameState.player.left);

    if (gameState.player.received != 0){
      mvprintw(rowPosition, colPosition, "GOLD received: %d", gameState.player.received);
    }
  }
  else{
    mvprintw(0,0, "Spectator: %d nuggests unclaimed.", gameState.player.left);
  }

  refresh(); 
}

void handle_error(const char* message){
  sscanf(message, "ERROR %s", errorMessage);
  mvprintw(0, colPosition, "%s", errorMessage);
  refresh();
}

void handle_quit(const char* message){
  sscanf(message, "QUIT %s", quitMessage);

  endwin();   //exit curses
  printf("%s", quitMessage);  //print explanation
  printf("\n");  
}
