#ifndef POKESHELL_SAVE_H
#define POKESHELL_SAVE_H

#include "game.h"

// save the game to a file
// returns 1 on success, 0 on failure
int save_game(const Game *game, const char *filename);

// load the game from a file
// returns 1 on success, 0 on failure
int load_game(Game *game, const char *filename);

#endif
