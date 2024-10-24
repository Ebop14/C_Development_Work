# CS50 Nuggets
## Design Spec
### Bashybunch, Fall 2023

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes `Communication` and `Display` modules on the *client* side, and `Communication`, `Logging`, and `GameState` modules on the *server* side.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Player

The *client* acts in one of two modes:

 1. *spectator*, the passive spectator mode described in the requirements spec.
 2. *player*, the interactive game-playing mode described in the requirements spec.

### User interface

See the requirements spec for both the command-line and interactive UI.

### Inputs and outputs

All input happens on the client-side, and is then sent to the server. Input takes the form of *keystrokes*.

A *spectator* has one valid keystroke:

+ `Q` quit the game.

A *player* has the following valid keystrokes:

+ `Q` quit the game.
+ `h` move left, if possible.
+ `l` move right, if possible.
+ `j` move down, if possible.
+ `k`	move up, if possible.
+ `y`	move diagonally up and left, if possible.
+ `u`	move diagonally up and right, if possible.
+ `b`	move diagonally down and left, if possible.
+ `n`	move diagonally down and right, if possible.

The client outputs to `stdout`:

+ An error message if the user inputs invalid command-line arguments.
+ Notifications and messages from the server related to gameplay.
+ Any quit, disconnect, or game-over messages.

The client renders the diplay showing the player's visible grid, with the status message at the top.

The client does not output an error message if the user inputs an invalid keystroke. Instead, the keystroke is rejected and never sent to the server.

The client logs other useful information not required for gameplay to `stderr`.

### Functional decomposition into modules

+ `Communication` Module: Manages all communication with the server.
+ `Display` Module: Responsible for displaying the game state to the player.
 
### Pseudo code for logic/algorithmic flow

The client will run as follows:

```
  client starts
  validate command-line arguments
  initialize Communication module
  display game grid
  while game not ended:
    get keystroke
    if keystroke valid:
        send to server via Communication module
    update display using Display module
  end
```

#### validateArguments

```
  retrieve command-line arguments
  if argument count invalid:
    exit error
  if hostname invalid:
    exit error
  if port invalid:
    exit error
  if playername provided:
    if playername invalid:
      exit error
  success
```

### Major data structures

+ `PlayerState`: Holds the current state of the player, including position, gold count, etc.
+ `VisibleGrid`: A subset of the main grid, which is a 2D array, representing what a player can currently see.

---

## Server
### User interface

See the requirements spec for the command-line interface.
There is no interaction with the user.

### Inputs and outputs

The server takes in a *map* file in `.txt` format as input and outputs game state changes to the *terminal*.

The server logs any errors encountered to `stderr`.

### Functional decomposition into modules

+ `GameState` Module: Keeps track of the overall game state.
+ `Communication` Module: Handles incoming and outgoing messages.
+ `Logging` Module: Handles error and game logs.

### Pseudo code for logic/algorithmic flow

The server will run as follows:

```
  server starts
  initialize GameState from map file
  initialize Communication module
  while game not ended:
    listen for client message
    update GameState based on message
    broadcast updated GameState to all clients
    log changes using Logging module
  end
  cleanup
```

### Major data structures

+ `GameState`: Represents the game map in its current state. Contains info about gold, players' positions, etc.

---

## Display module

### Functional decomposition

+ `renderGrid`: Displays the game grid to the player.
+ `renderPlayerStats`: Shows the player's current statistics like gold count.
+ `renderNotifications`: Displays in-game notifications or messages.

### Pseudo code for logic/algorithmic flow

### renderGrid

```
  get VisibleGrid from server
  display it to the user
```

#### renderPlayerStats

```
  get PlayerState from server
  display relevant stats to the user
```

#### renderNotifications

```
  if new notifications available:
    display them
```

### Major data structures

`RenderQueue`: A queue that holds elements to be rendered next.


## Communication module (Client)

### Functional decomposition

+ `connectToServer`: Establishes a connection to the server.
+ `sendInput`: Sends the player's input to the server after validating.
+ `receiveUpdate`: Listens for and receives game state updates from the server.
+ `disconnect`: Properly closes the connection to the server.
+ `handleDisconnect`: Cleanly handles disconnects by informing user and attempting to reconnect.

### Pseudo code for logic/algorithmic flow

#### connectToServer

```
  try to establish a connection to the server using server's IP and port
  if successful:
    return success
  else:
    return error
```

#### sendInput

```
  validate and package player's input into a message format
  send message to server
  if error in sending:
    logError("Failed to send input to server")
```

#### receiveUpdate

```
  while game not ended or connection not closed:
    try:
      listen for incoming data from server
      if data received:
        process data
        if data is proper:
          update local game state using data
        else:
          log error
      else:
          log connection error
    except ConnectionError:
      handleDisconnect()
      break
```

#### disconnect

```
  close the connection to the server
  if error in closing:
    logError("Failed to disconnect from server")
```

#### handleDisconnect
```
  inform the user about the disconnect
  attempt to reconnect
  if reconnect fails or not allowed:
    perform cleanup
    update game state to reflect disconnect status
    exit client or offer option to retry connection
```

### Major data structures

+ `ServerEndpoint`: Contains the IP address and port of the server.
+ `LocalGameState`: A cached version of the game state on the client side, updated whenever new data is received from the server. This cache ensures that the client can continue to display the game even if there's a temporary interruption in receiving updates from the server.

*Note:* The `LocalGameState` is essential in scenarios where there might be network lags or issues. It ensures the user interface remains responsive and provides the most recent valid state of the game to the player.


## GameState module

### Functional decomposition

+ `initializeGame`: Initialize the game grid and set everything to a starting state.
+ `addPlayer`: Add a new player to the game.
+ `removePlayer`: Remove a player from the game.
+ `addSpectator`: Add a spectator to the game.
+ `updatePlayerPosition`: Update the position of a player on the grid.
+ `getVisibleGridForPlayer`: Get a subset of the main grid that a particular player can currently see.
+ `checkForGold`: Check if a player's new position is on a gold nugget.
+ `endGame`: Check game-ending conditions and end the game if necessary.

### Pseudo code for logic/algorithmic flow

#### initializeGame

```
  read map file
  for each row in map file:
    for each cell in row:
      if cell contains gold:
          increment total_gold
      add cell to Grid
```

#### addPlayer

```
  if maximum players not reached:
    generate unique letter for player
    find a valid starting position on the Grid
    create a new Player instance with starting position, name, letter, and other initial attributes
    add Player to ActivePlayers list
    return success
  else:
    return error (maximum players reached)
```

#### removePlayer

```
  find player in ActivePlayers list using their unique letter
  if player found:
    remove player from ActivePlayers
    update Grid to reflect player's removal
    return success
  else:
    return error (player not found)
```

#### addSpectator

```
  if there is an active spectator:
    remove active spectator and send quit message
 
  create new Spectator instance
  set ActiveSpectator to the new instance
  return success
```

#### updatePlayerPosition

```
  get player's desired new position based on input
  if new position is valid and unoccupied:
    update player's position in Player instance
    update Grid to reflect the change
    if new position contains gold:
      increment player's gold count
      decrement remaining_gold
    return success
  else:
    return error (invalid move)
```

#### getVisibleGridForPlayer

```
  initialize empty visibility_grid
  calculate bounds of visible area based on player's current position
  for each cell in the bounds:
    add cell's state to visibility_grid
  return visibility_grid
```

#### checkForGold

```
  if player's position contains gold:
    remove gold from Grid
    increment player's purse
    decrement remaining_gold
```

#### endGame

```
  if remaining_gold is 0 or other end conditions met:
    set game status to ended
    broadcast game over message to all clients
```

### Major data structures

`Grid`: Represents the game grid.
+ *Attributes*:
  + `rows` and `cols`: Grid dimensions.
  + `cells`: A 2D array or list of lists representing the grid, where each cell might contain information about its state, e.g., whether it contains gold, a player, or is empty.

`Player`: Represents a player in the game.
+ *Attributes*:
  + `real_name`: Original name provided by the player.
  + `formatted_name`: The name after truncation and replacement of disallowed characters.
  + `letter`: Assigned player letter (A, B, ... Z).
  + `position`: Current position on the grid.
  + `purse`: Number of gold nuggets the player has collected.
  + `endpoint`: Information about the client's IP address and port for communication.
  + `visibility_grid`: A grid representing areas of the map the player can currently see.

`Spectator`: Represents a spectator in the game.
+ *Attributes*:
  + `endpoint`: Information about the client's IP address and port.

`GameState`:
+ *Attributes*:
  + `grid`: Instance of the `Grid` structure.
  + `players`: A `hashmap` that maps letters (A..Z) to instances of the `Player` structure.
  + `spectator`: Instance of the `Spectator` structure.
  + `total_gold`: Total supply of gold in the game.
  + `remaining_gold`: Gold nuggets not yet collected.

`ActivePlayers`: A list of currently connected players and their states.

`ActiveSpectator`: The spectator currently connected.


## Communication module (Server)

### Functional decomposition

+ `sendMessage`: Sends a message to a specified client.
+ `broadcastMessage`: Sends a message to all connected clients.
+ `listen`: Listens for incoming messages.
+ `handleClientDisconnect`: Properly handles a client disconnect.

### Pseudo code for logic/algorithmic flow

#### sendMessage:

```
	package data
	send data to specified client
```

#### listen:

```
	while game not ended:
	    wait for incoming data
	    process data
      if malformatted message:
        log error and respond to client
```

#### broadcastMessage:

```
	package data
	for each client:
	    send data
```

#### handleClientDisconnect

```
  remove the player from the game state
  broadcast message to other players informing of the disconnect
  log the disconnect event
  close the player's connection socket
```

### Major data structures

+ `MessageQueue`: Holds messages that are to be processed in a queue.
+ `EndpointList`: A list of all currently connected clients' endpoints.


## Logging module

+ `logMessage`: Logs a given message with a timestamp.
+ `logError`: Specifically logs error messages.
+ `writeToFile`: Writes logs to a specified file.
+ `displayLog`: Outputs the log to the terminal.

### Functional decomposition

#### logMessage

```
  get current timestamp
  append message with timestamp to log list
```

#### logError

```
  get current timestamp
  append error message with timestamp to log list with error tag
```

#### writeToFile

```
  for each message in log list:
    write message to specified file
```

#### displayLog

```
  for each message in log list:
    print message to terminal
```

### Pseudo code for logic/algorithmic flow

### Major data structures

+ `LogList`: A list or collection holding all log messages.
