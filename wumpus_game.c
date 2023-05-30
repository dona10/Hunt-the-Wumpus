/*
	Program 2: Wumpus (bats, arrow and all arrays are dynamic)
	CS 211, UIC, Fall 2022
	System: Replit
	Author: Dona Maria
This program implements the wumpus game, with superbats or arrows and using dynamic array.
*/

#include <stdio.h>
#include <stdbool.h>	// for the bool type in C
#include <stdlib.h>		// for srand
#include <ctype.h>		// for toupper()

// global constants
#define MAX_LINE_LENGTH 81
#define NUMBER_OF_ROOMS 20

// Used to more conveniently pass all game information between functions.
struct GameInfo {
	int moveNumber;  // Counts up from 1, incrementing for each move
	int personRoom;  // Room 1..20 the person currently is in
	int wumpusRoom;  // Room 1..20 the Wumpus is in
	int pitRoom1;    // Room 1..20 the first pit is in
	int pitRoom2;    // Room 1..20 the second pit is in
  int batRoom1;
  int batRoom2;
  int arrow;
};

// Function prototype needed to allow calls in any order between
//   functions checkForHazards() and inRoomWithBats()
void checkForHazards(struct GameInfo *theGameInfo, bool *personIsAlive, int ** rooms);

//--------------------------------------------------------------------------------
void displayCave()
{
    printf( "\n"
			"       ______18______             \n"
    		"      /      |       \\           \n"
    		"     /      _9__      \\          \n"
    		"    /      /    \\      \\        \n"
    		"   /      /      \\      \\       \n"
    		"  17     8        10     19       \n"
    		"  | \\   / \\      /  \\   / |    \n"
    		"  |  \\ /   \\    /    \\ /  |    \n"
    		"  |   7     1---2     11  |       \n"
    		"  |   |    /     \\    |   |      \n"
    		"  |   6----5     3---12   |       \n"
    		"  |   |     \\   /     |   |      \n"
    		"  |   \\       4      /    |      \n"
    		"  |    \\      |     /     |      \n"
    		"  \\     15---14---13     /       \n"
    		"   \\   /            \\   /       \n"
    		"    \\ /              \\ /        \n"
    		"    16---------------20           \n"
    		"\n");
}


//--------------------------------------------------------------------------------
void displayInstructions()
{
    printf( "Hunt the Wumpus:                                             \n"
    		"The Wumpus lives in a completely dark cave of 20 rooms. Each \n"
    		"room has 3 tunnels leading to other rooms.                   \n"
    		"                                                             \n"
    		"Hazards:                                                     \n"
        	"1. Two rooms have bottomless pits in them.  If you go there you fall and die.   \n"
        	"2. Two other rooms have super-bats.  If you go there, the bats grab you and     \n"
        	"   fly you to some random room, which could be troublesome.  Then those bats go \n"
        	"   to a new room different from where they came from and from the other bats.   \n"
    		"3. The Wumpus is not bothered by the pits or bats, as he has sucker feet and    \n"
        	"   is too heavy for bats to lift.  Usually he is asleep.  Two things wake       \n"
        	"    him up: Anytime you shoot an arrow, or you entering his room.  The Wumpus   \n"
        	"    will move into the lowest-numbered adjacent room anytime you shoot an arrow.\n"
        	"    When you move into the Wumpus' room, then he wakes and moves if he is in an \n"
        	"    odd-numbered room, but stays still otherwise.  After that, if he is in your \n"
        	"    room, he snaps your neck and you die!                                       \n"
        	"                                                                                \n"
        	"Moves:                                                                          \n"
        	"On each move you can do the following, where input can be upper or lower-case:  \n"
        	"1. Move into an adjacent room.  To move enter 'M' followed by a space and       \n"
        	"   then a room number.                                                          \n"
        	"2. Shoot your guided arrow through a list of up to three adjacent rooms, which  \n"
        	"   you specify.  Your arrow ends up in the final room.                          \n"
        	"   To shoot enter 'S' followed by the number of rooms (1..3), and then the      \n"
        	"   list of the desired number (up to 3) of adjacent room numbers, separated     \n"
        	"   by spaces. If an arrow can't go a direction because there is no connecting   \n"
        	"   tunnel, it ricochets and moves to the lowest-numbered adjacent room and      \n"
        	"   continues doing this until it has traveled the designated number of rooms.   \n"
        	"   If the arrow hits the Wumpus, you win! If the arrow hits you, you lose. You  \n"
        	"   automatically pick up the arrow if you go through a room with the arrow in   \n"
        	"   it.                                                                          \n"
        	"3. Enter 'R' to reset the person and hazard locations, useful for testing.      \n"
        	"4. Enter 'C' to cheat and display current board positions.                      \n"
        	"5. Enter 'D' to display this set of instructions.                               \n"
        	"6. Enter 'P' to print the maze room layout.                                     \n"
        	"7. Enter 'X' to exit the game.                                                  \n"
        	"                                                                                \n"
        	"Good luck!                                                                      \n"
        	" \n\n");
}//end displayInstructions()


//--------------------------------------------------------------------------------
// Return true if randomValue is already in array
int alreadyFound(int randomValue,      // New number we're checking
                 int randomNumbers[],  // Set of numbers previously found
                 int limit)            // How many numbers previously found
{
    int returnValue = false;
    
    // compare random value against all previously found values
    for( int i = 0; i<limit; i++) {
        if( randomValue == randomNumbers[i]) {
            returnValue = true;   // It is already there
            break;
        }
    }
    return returnValue;
} 


//--------------------------------------------------------------------------------
// Fill this array with unique random integers 1..20
void setUniqueValues(int randomNumbers[],   // Array of random numbers
                     int size)              // Size of random numbers array
{
    int randomValue = -1;
    
    for( int i = 0; i<size; i++) {
        do {
            randomValue = rand() % NUMBER_OF_ROOMS + 1;   // random number 1..20
        } while (alreadyFound(randomValue, randomNumbers, i));
        randomNumbers[i] = randomValue;
    }
} 


//--------------------------------------------------------------------------------
// Set the Wumpus, player, bats and pits in distinct random rooms
void initializeGame(struct GameInfo *gameInfo)   // All game settings variables
{
    // Array of 6 unique values 1..20, to be used in initializing cave hazards locations
    int *randomNumbers = (int*) malloc(7*sizeof(int));
    
    // Initialize cave room connections
    //       ______18______
    //      /      |       \
    //     /      _9__      \
    //    /      /    \      \
    //   /      /      \      \
    //  17     8        10    19
    // |  \   / \      /  \   / |
    // |   \ /   \    /    \ /  |
    // |    7     1---2     11  |
    // |    |    /     \    |   |
    // |    6----5     3---12   |
    // |    |     \   /     |   |
    // |    \       4      /    |
    // |     \      |     /     |
    //  \     15---14---13     /
    //   \   /            \   /
    //    \ /              \ /
    //    16---------------20
    
      
    // Select some unique random values 1..20 to be used for 2 bats rooms, 2
    // pits rooms, Wumpus room, and initial player room
    setUniqueValues(randomNumbers, 7);
    // Use the unique random numbers to set initial locations of hazards, which
    //    should be non-overlapping.
    gameInfo->personRoom = randomNumbers[0];   
    gameInfo->wumpusRoom = randomNumbers[1];
    gameInfo->pitRoom1 = randomNumbers[2];
    gameInfo->pitRoom2 = randomNumbers[3];
    gameInfo->batRoom1 = randomNumbers[4];
    gameInfo->batRoom2 = randomNumbers[5];
    gameInfo->arrow = randomNumbers[6];
    gameInfo->moveNumber = 1;
    free(randomNumbers);
}// end initializeBoard(...)


//--------------------------------------------------------------------------------
// Returns true if nextRoom is adjacent to current room, else returns false.
int roomIsAdjacent( int tunnels[ 3],     // Array of adjacent tunnels
                    int nextRoom)        // Desired room to move to
{
    return( tunnels[0] == nextRoom ||
            tunnels[1] == nextRoom ||
            tunnels[2] == nextRoom
          );
}


//--------------------------------------------------------------------------------
// Display where everything is on the board.
void displayCheatInfo(struct GameInfo gameInfo)
{
    printf( "Cheating! Game elements are in the following rooms: \n"
    		"Player Wumpus Pit1 Pit2 Bats1 Bats2 Arrow  \n"
    		"%4d %5d %6d %5d %5d %5d %5d \n\n", 
			gameInfo.personRoom,
			gameInfo.wumpusRoom,
			gameInfo.pitRoom1,
			gameInfo.pitRoom2,
      gameInfo.batRoom1,
      gameInfo.batRoom2,
      gameInfo.arrow
		);
}// end displayCheatInfo(...)


//--------------------------------------------------------------------------------
// Display room number and hazards detected
void displayRoomInfo( struct GameInfo gameInfo, int ** rooms)
{
    // Retrieve player's current room number and display it
    int currentRoom = gameInfo.personRoom;
    printf("You are in room %d. ", currentRoom);
    
    // Retrieve index values of all 3 adjacent rooms
    int room1 = rooms[currentRoom][0];
    int room2 = rooms[currentRoom][1];
    int room3 = rooms[currentRoom][2];
    
    // Display hazard detection message if Wumpus is in an adjacent room
    int wumpusRoom = gameInfo.wumpusRoom;
    if( room1 == wumpusRoom || room2 == wumpusRoom || room3 == wumpusRoom) {
        printf("You smell a stench. ");
    }
    
    // Display hazard detection message if a pit is in an adjacent room
    int pit1Room = gameInfo.pitRoom1;
    int pit2Room = gameInfo.pitRoom2;
    if( room1 == pit1Room || room1 == pit2Room ||
        room2 == pit1Room || room2 == pit2Room ||
        room3 == pit1Room || room3 == pit2Room
      ) {
        printf("You feel a draft. ");
    }

    int bat1Room = gameInfo.batRoom1;
    int bat2Room = gameInfo.batRoom2;
    if( room1 == bat1Room || room1 == bat2Room ||
        room2 == bat1Room || room2 == bat2Room ||
        room3 == bat1Room || room3 == bat2Room
      ) {
        printf("You hear rustling of bat wings. ");
    }
  
    printf("\n\n");
}//end displayRoomInfo(...)


//--------------------------------------------------------------------------------
// Returns true if the room number passed is equal to the room of wumpus, person, bats, and arrows
bool sameRoom(int number, struct GameInfo *gameInfo)
{
    bool same = false;
    if (gameInfo->personRoom == number ||
        gameInfo->batRoom1 == number ||
        gameInfo->batRoom2 == number
    )
    same = true;
    return same;
}


//--------------------------------------------------------------------------------
// If the player just moved into a room with a pit, the person dies.
// If the person just moved into the Wumpus room, then if the room number is odd
// the Wumpus moves to a random adjacent room.
void checkForHazards(
         struct GameInfo *gameInfo, // Hazards location and game info
         bool *personIsAlive, int ** rooms)       // Person is alive, but could die depending on the hazards
{
    // retrieve the room the person is in
    int personRoom = gameInfo->personRoom;
  
    // Check for the Wumpus
    if( personRoom == gameInfo->wumpusRoom) {
        // To make it easier to test, in this version of the program the Wumpus always
        // moves if it is currently in an odd-numbered room, and it moves into the
        // lowest-numbered adjacent room.  In the version that is more fun to play
        // (but harder to test), the Wumpus has a 75% chance of moving, and a 25%
        // chance of staying and killing you.  The "more fun" code is commented out below.
        
        // Wumpus is there. 75% change of Wumpus moving, 25% chance of it killing you
        // Generate a random number 1..100
        // if(  (rand() % 100) < 75) {
        if( gameInfo->wumpusRoom %2 == 1) {
            // You got lucky and the Wumpus moves away
            printf( "You hear a slithering sound, as the Wumpus slips away. \n"
					"Whew, that was close! \n");
            // Choose a random adjacent room for the Wumpus to go into
            // gameInfo.wumpusRoom = rooms[ personRoom][ rand() % 3];
            gameInfo->wumpusRoom = rooms[ personRoom][ 0];  // Choose the lowest-numbered adjacent room
        }
        else {
            // Wumpus kills you
            printf(	"You briefly feel a slimy tentacled arm as your neck is snapped. \n"
					"It is over.\n");
            *personIsAlive = false;
            return;
        }
    }
  
    // Check whether there is a pit
    if( personRoom == gameInfo->pitRoom1 || personRoom == gameInfo->pitRoom2) {
        // Person falls into pit
        printf("Aaaaaaaaahhhhhh....   \n");
        printf("    You fall into a pit and die. \n");
        *personIsAlive = false;
        return;
    }

   // check whether there is bats in the room
    if( personRoom == gameInfo->batRoom1) {
        while (sameRoom(personRoom, gameInfo))
          personRoom = rand() % 20 + 1; 
        gameInfo->personRoom = personRoom;
        int bat1 = gameInfo->batRoom1;
        printf("Woah... you're flying!");
        while (sameRoom(bat1, gameInfo))
          bat1 = rand() % 20 + 1; 
        gameInfo->batRoom1 = bat1;
        printf("\nYou've just been transported by bats to room %d.\n", gameInfo->personRoom);
    }

  // check whether there is bats in the room
    if( personRoom == gameInfo->batRoom2) {
        while (sameRoom(personRoom, gameInfo))
          personRoom = rand() % 20 + 1; 
        gameInfo->personRoom = personRoom;
        int bat2 = gameInfo->batRoom2;
        printf("Woah... you're flying!");
        while (sameRoom(bat2, gameInfo))
          bat2 = rand() % 20 + 1; 
        gameInfo->batRoom2 = bat2;
        printf("\nYou've just been transported by bats to room %d.\n", gameInfo->personRoom);
    }
  
    // check whether there is an arrow in the room
    if( personRoom == gameInfo->arrow) {
        printf("Congratulations, you found the arrow and can once again shoot.\n");
        gameInfo->arrow = -1;
   }
}//end checkForHazards(...)


//--------------------------------------------------------------------------------
// Prompt for and reset the positions of the game hazards and the person's location,
// useful for testing.  No error checking is done on these values.
void resetPositions(struct GameInfo *theGameInfo)
{
    printf("Enter the room locations (1..20) for player, wumpus, pit1, pit2, bats1, bats2, and arrow: \n");
	// In the scanf below note that we put the space at the beginning of the scanf so  
    // that any newline left over from a previous input is not read in and used as 
	// the next move. Another option is having getchar() after the scanf() statement.
	scanf(" %d %d %d %d %d %d %d", &theGameInfo->personRoom, &theGameInfo->wumpusRoom, 
                          &theGameInfo->pitRoom1, &theGameInfo->pitRoom2, &theGameInfo->batRoom1, &theGameInfo->batRoom2, &theGameInfo->arrow);
    printf("\n");
}


//--------------------------------------------------------------------------------
// To set the room numbers
void setRooms(int **rooms)
{
  rooms[0][0] = 0;
  rooms[0][1] = 0;
  rooms[0][2] = 0;
  rooms[1][0] = 2;
  rooms[1][1] = 5;
  rooms[1][2] = 8;
  rooms[2][0] = 1;
  rooms[2][1] = 3;
  rooms[2][2] = 10;
  rooms[3][0] = 2;
  rooms[3][1] = 4;
  rooms[3][2] = 12;
  rooms[4][0] = 3;
  rooms[4][1] = 5;
  rooms[4][2] = 14;
  rooms[5][0] = 1;
  rooms[5][1] = 4;
  rooms[5][2] = 6;
  rooms[6][0] = 5;
  rooms[6][1] = 7;
  rooms[6][2] = 15;
  rooms[7][0] = 6;
  rooms[7][1] = 8;
  rooms[7][2] = 17;
  rooms[8][0] = 1;
  rooms[8][1] = 7;
  rooms[8][2] = 9;
  rooms[9][0] = 8;
  rooms[9][1] = 10;
  rooms[9][2] = 18;
  rooms[10][0] = 9;
  rooms[10][1] = 11;
  rooms[10][2] = 19;
  rooms[11][0] = 10;
  rooms[11][1] = 12;
  rooms[11][2] = 19;
  rooms[12][0] = 3;
  rooms[12][1] = 11;
  rooms[12][2] = 13;
  rooms[13][0] = 12;
  rooms[13][1] = 14;
  rooms[13][2] = 20;
  rooms[14][0] = 4;
  rooms[14][1] = 13;
  rooms[14][2] = 15;
  rooms[15][0] = 6;
  rooms[15][1] = 14;
  rooms[15][2] = 16;
  rooms[16][0] = 15;
  rooms[16][1] = 17;
  rooms[16][2] = 20;
  rooms[17][0] = 7;
  rooms[17][1] = 16;
  rooms[17][2] = 18;
  rooms[18][0] = 9;
  rooms[18][1] = 17;
  rooms[18][2] = 19;
  rooms[19][0] = 11;
  rooms[19][1] = 18;
  rooms[19][2] = 20;
  rooms[20][0] = 13;
  rooms[20][1] = 16;
  rooms[20][2] = 19;
}


//--------------------------------------------------------------------------------
// checks if the person has the arrow
// if the person does, then reads in number of rooms it has to pass and the room numbers
// ricochets if the room numbers are not adjacent
// Wumpus moves to the lowest adjacent room everytime the shoot runs
void shootArrow(struct GameInfo *gameInfo, int ** rooms, bool *wumpusIsAlive, bool *personIsAlive) {
    int numRooms ;
    int *arrowRooms = (int*) malloc(3*sizeof(int));

    // checks if the person has the arrow
    if (gameInfo->arrow != -1)
    {
      printf("Sorry, you can't shoot an arrow you don't have. Go find it.\n");
      return;
    }
    gameInfo->arrow = gameInfo->personRoom;
    printf("Enter the number of rooms (1..3) into which you want to shoot, followed by the rooms themselves: ");
    scanf(" %d", &numRooms);

    // tp check if the number read is in between 1 and 3
    if (numRooms > 3 || numRooms < 1)
    {
      printf("Sorry, the max number of rooms is 3.  Setting that value to 3.\n");
    numRooms = 3;
    }

    for (int i = 0; i < numRooms; i++) 
    {
      scanf(" %d", &arrowRooms[i]);

      // if the room number given is adjacent to the current room
      if (roomIsAdjacent(rooms[gameInfo->arrow], arrowRooms[i]))
      {
        gameInfo->arrow = arrowRooms[i];
        if (gameInfo->arrow == gameInfo->wumpusRoom)
        {
          *wumpusIsAlive = false;
          printf("Wumpus has just been pierced by your deadly arrow!\nCongratulations on your victory, you awesome hunter you.\n");
          return;
        }
        if (gameInfo->arrow == gameInfo->personRoom)
        {
          *personIsAlive = false;
          printf("You just shot yourself. \nMaybe Darwin was right. You're dead.");
          return;
        }
      }

      // if the arrow ricochets  
      else
      {
        printf("Room %d is not adjacent. Arrow ricochets...\n", arrowRooms[i]);
        gameInfo->arrow = rooms[gameInfo->arrow][0];
        if (gameInfo->arrow == gameInfo->wumpusRoom)
        {
          *wumpusIsAlive = false;
          printf("Your arrow ricochet killed the Wumpus, you lucky dog!\nAccidental victory, but still you win!");
          return;
        }
        if (gameInfo->arrow == gameInfo->personRoom)
        {
          *personIsAlive = false;
          printf("You just shot yourself, and are dying.\nIt didn't take long, you're dead.\n");
          return;
        }
      } 
    }

    // to move the wumpus to the lowest adjacent room
    gameInfo->wumpusRoom = rooms[gameInfo->wumpusRoom][0];
     free(arrowRooms);
}


//--------------------------------------------------------------------------------
int main(void) {
    struct GameInfo gameInfo;   // Used to more easily pass game info variables around
    bool personIsAlive = true;  // Used in checking for end of game
    bool wumpusIsAlive = true;  // Used in checking for end of game
    char typeOfMove;            // Used to handle user input letter
    int nextRoom;               // User input of destination room number, used on a 'M' type move
    int **rooms = (int**) malloc (21*(sizeof(int*)));
  for (int i = 0; i<21;i++){
    rooms[i]= (int *)malloc(3*sizeof(int));
  }
  setRooms(rooms);
    // Seed the random number generator.  Change seed to time(0) to change output each time.
    // srand(time(0));
    srand(1);
    
    // Set random initial values for person, Wumpus, bats and pits
    initializeGame( &gameInfo);

	// Main playing loop.  Break when player dies, or player kills Wumpus
    while (personIsAlive && wumpusIsAlive) {
            
		// Display current room information: Room number, hazards detected
		displayRoomInfo( gameInfo, rooms);
		
		// Prompt for and handle move
		printf("%d. Enter your move (or 'D' for directions): ", gameInfo.moveNumber);
        // Note the extra space in the scanf below between the opening quote " and the %c.  
        //    This skips leading white space in the input so that the newline left over from
        //    a previous move is not read in and used as the current move.  An alternative 
        //    is using getchar() after the scanf() statement.
		scanf(" %c", &typeOfMove);	
									
		typeOfMove = toupper(typeOfMove);  // Make uppercase to facilitate checking
		
		// Check all types of user input and handle them.  This uses if-else-if code
		//   rather than switch-case, so that we can take advantage of break and continue.
		if( typeOfMove == 'D') {
			displayCave();
			displayInstructions();
			continue;       // Loop back up to reprompt for the same move
		}
		else if( typeOfMove == 'P') {
			// To assist with play, display the cave layout
			displayCave();
			continue;       // Loop back up to reprompt for the same move
		}
		else if( typeOfMove == 'M') {
			// Move to an adjacent room,
            // Note the extra space in the scanf below between the opening quote " and the %c.  
            //    This skips leading white space in the input so that the newline left over from
            //    a previous move is not read in and used as the current move.  An alternative 
            //    is using getchar() after the scanf() statement.
			scanf(" %d", &nextRoom);
			
			if( roomIsAdjacent( rooms[gameInfo.personRoom], nextRoom)) {
				gameInfo.personRoom = nextRoom;        // move to a new room
				// Check if pit or wumpus is present in this new room
				checkForHazards(&gameInfo, &personIsAlive, rooms);
			}
			else {
				printf("Invalid move.  Please retry. \n");
				continue;        // Doesn't count as a move, so retry same move.
			}
		}
		else if( typeOfMove == 'C') {
			// Display Cheat information
			displayCheatInfo(gameInfo);
			continue;        // Doesn't count as a move, so retry same move.
		}
		else if( typeOfMove == 'R') {
			// Specify resetting the hazards and person positions, useful for testing
			resetPositions( &gameInfo);
			continue;        // Doesn't count as a move, so retry same move.
		}
        else if( typeOfMove == 'G') {
			// Prompt for room number guess and see if user has found the Wumpus
            int wumpusRoomGuess = 0;
			printf("Enter room (1..20) you think Wumpus is in: ");
            scanf(" %d", &wumpusRoomGuess);
            // See if userguess was correct, for a win or loss.
            if( gameInfo.wumpusRoom == wumpusRoomGuess) {
                printf("You won!\n");
            }
            else{ 
                printf("You lost.\n");
            }
			break;
		}

    else if (typeOfMove == 'S') {
      shootArrow(&gameInfo, rooms, &wumpusIsAlive, &personIsAlive);
    }
      
		else if( typeOfMove == 'X') {
			// Exit program
			personIsAlive = false;   // Indicate person is dead as a way to exit playing loop
			break;
		}

		// Increment the move number
		gameInfo.moveNumber = gameInfo.moveNumber + 1;
        
    }//end while(personIsAlive && wumpusIsAlive)
    
    printf("\nExiting Program ...\n");
	 
   for (int i = 0; i<21;i++)
     free(rooms[i]);
   free(rooms);

	return 0;
}
