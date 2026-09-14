#ifndef POKESHELL_MOVE_H
#define POKESHELL_MOVE_H

#include "types.h"

// a move a pokemon can use
typedef struct {
    char name[32];
    Type type;
    int power;
    int accuracy;    // out of 100
    int pp;
    int max_pp;
} Move;

// create a move with default values
Move move_create(const char *name, Type type, int power, int accuracy, int pp);

// load all moves from data file
// returns number of moves loaded
int move_load_all(Move *buffer, int max_moves);

// get the starting moves for a starter pokemon by name
// populates moves[] and sets count
void move_get_starter_moves(const char *starter_name, Move moves[], int *count);

#endif
