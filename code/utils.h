#ifndef __UTILS__
#define __UTILS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>


#define SIZE 4
#define _XOPEN_SOURCE 500

/**
 * Data structure containing the information about the game state
 * Variables have the same name as the global variables in pacman.c 
 * representing the state of the game.
 */
struct state_s{                                                                                                    //value: -1:up/left 1:down/right
    int Loc[5][2];                    //Location of Ghosts and Pacman (Pacman loc in idx 4) [0 1 2 3 = ghosts 4 = pacman][0 = up/down 1= right/left]
    int Dir[5][2];                    //Direction of Ghosts and Pacman [0 1 2 3 = ghosts 4 = pacman][0 = up/down 1= right/left]
    int StartingPoints[5][2];         //Default location in case Pacman/Ghosts die  [0 1 2 3 = ghosts 4 = pacman][0 = up/down 1= right/left]
    int Invincible;                       //Check for invincibility
    int Food;                             //Number of pellets left in level
    int Level[29][28];                //Main level array
    int LevelNumber;                      //What level number are we on?
    int GhostsInARow;                     //Keep track of how many points to give for eating ghosts
    int tleft;                            //How long left for invincibility
    int Points;                     //Initial points
    int Lives;       
};

typedef struct state_s state_t;


/**
* Move type
*/
typedef enum moves{
	left=0,
	right=1,
	up=2,
	down=3
} move_t;

/**
 * Back Propagation type 
 */
typedef enum propagation{
	max=0,
	avg=1
} propagation_t;



/**
 * Executes an action, updates the board and the score, and return true if the direction of Pacman has changed,
 * and false otherwise
 */
bool execute_move_t(state_t* state, move_t move);
void MovePacmanSim(state_t* state);
void MoveGhostsSim(state_t* state);
void CheckCollisionSim(state_t* state);



#endif
