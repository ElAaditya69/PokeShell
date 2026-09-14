#ifndef POKESHELL_GAME_H
#define POKESHELL_GAME_H

#include "pokemon.h"
#include "player.h"
#include "world.h"

// game states
typedef enum {
    STATE_TITLE,
    STATE_STARTER_SELECT,
    STATE_EXPLORE,
    STATE_BATTLE,
    STATE_MENU,
    STATE_GAME_OVER,
    STATE_WIN
} GameState;

#define GYM_TEAM_SIZE 3

// the entire game state
typedef struct {
    GameState state;
    Player player;
    Map current_map;
    Pokemon wild_pokemon;
    int battle_turn;   // 0 = player turn, 1 = enemy turn

    /* gym leader battle */
    int is_gym_battle;
    Pokemon gym_team[GYM_TEAM_SIZE];
    int gym_team_size;
    int gym_current;   /* index of the gym leader's active pokemon */
} Game;

// initialize the game
void game_init(Game *game);

// main game loop
void game_run(Game *game);

// free any allocated resources
void game_cleanup(Game *game);

#endif
