// Some variables that you may want to change

#ifndef DATAROOTDIR
#    define DATAROOTDIR "."
#endif
#define LEVELS_FILE DATAROOTDIR "/Levels/level__.dat"

#include "utils.h"

#define EXIT_MSG "Good bye!"
#define END_MSG "Game Over"
#define QUIT_MSG "Bye"
#define LEVEL_ERR "Cannot find level file: "
#define LEVEL_WIDTH 28
#define LEVEL_HEIGHT 29
#define HOW_SLOW 3                     //How slow vulnerable ghost move

extern int total_generated_nodes;
extern int total_expanded_nodes;
extern int total_max_depth;
extern clock_t time_begin;
void DrawWindowState(state_t state);                         //Refresh display

