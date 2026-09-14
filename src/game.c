#include "game.h"
#include "ui.h"
#include "battle.h"
#include "world.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void game_init(Game *game)
{
    game->state = STATE_TITLE;
    game->battle_turn = 0;
    memset(&game->player, 0, sizeof(Player));
    memset(&game->current_map, 0, sizeof(Map));
    memset(&game->wild_pokemon, 0, sizeof(Pokemon));
}

void game_run(Game *game)
{
    // TODO: implement the state machine loop
    // for now, just print something so we know it works
    (void)game;
    ui_print_title();
    printf("Game initialized. Ready to go!\n");
}

void game_cleanup(Game *game)
{
    (void)game;  // nothing to free yet
}
