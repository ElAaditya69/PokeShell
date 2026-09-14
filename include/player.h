#ifndef POKESHELL_PLAYER_H
#define POKESHELL_PLAYER_H

#include "pokemon.h"

#define MAX_TEAM 6
#define START_POKEBALLS 10

// the player
typedef struct {
    char name[32];
    Pokemon team[MAX_TEAM];
    int team_size;
    int pokeballs;
    int badges;
    int pos_x;
    int pos_y;
} Player;

// create a new player with a name
Player player_create(const char *name);

// add a pokemon to the team (returns 1 if added, 0 if team full)
int player_add_pokemon(Player *player, Pokemon pokemon);

// remove a pokemon from the team by index
void player_remove_pokemon(Player *player, int index);

// use a pokeball (decrements count, returns 1 if you had one)
int player_use_pokeball(Player *player);

// check if all pokemon in the team have fainted
int player_all_fainted(const Player *player);

// heal all pokemon in the team (pokemon center)
void player_heal_team(Player *player);

#endif
