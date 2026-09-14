#include "game.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void game_init(Game *game)
{
    memset(game, 0, sizeof(Game));
    game->state = STATE_TITLE;
    game->current_map = world_create_starting_map();
    game->player = player_create("Ash");
}

void game_run(Game *game)
{
    srand((unsigned int)time(NULL));
    game->state = STATE_TITLE;

    while (game->state != STATE_GAME_OVER) {
        switch (game->state) {

        case STATE_TITLE:
            ui_clear();
            ui_print_title();
            game->state = STATE_STARTER_SELECT;
            break;

        case STATE_STARTER_SELECT: {
            int choice = ui_starter_select();

            // starter data: name, type, base_hp, base_atk, base_def
            const char *names[]  = {"Bulbasaur", "Charmander", "Squirtle"};
            Type types[]         = {TYPE_GRASS,  TYPE_FIRE,    TYPE_WATER};
            int base_hp[]        = {45,          39,           44};
            int base_atk[]       = {49,          52,           48};
            int base_def[]       = {49,          43,           65};

            Pokemon starter = pokemon_create_starter(
                names[choice], types[choice],
                base_hp[choice], base_atk[choice], base_def[choice]);
            player_add_pokemon(&game->player, starter);

            ui_clear();
            printf("  You chose %s!\n", starter.name);
            ui_wait();
            game->state = STATE_EXPLORE;
            break;
        }

        case STATE_EXPLORE:
            ui_clear();
            ui_print_map(&game->current_map, &game->player);

            if (player_all_fainted(&game->player)) {
                printf("  All your Pokemon have fainted!\n");
                ui_wait();
                game->state = STATE_GAME_OVER;
                break;
            }

            {
                char input = ui_get_key();
                if (input == 'q' || input == 'Q') {
                    game->state = STATE_GAME_OVER;
                    break;
                }

                int dx = 0, dy = 0;
                switch (input) {
                    case 'w': case 'W': dy = -1; break;
                    case 's': case 'S': dy =  1; break;
                    case 'a': case 'A': dx = -1; break;
                    case 'd': case 'D': dx =  1; break;
                }

                if (dx != 0 || dy != 0) {
                    world_move_player(&game->current_map,
                                      &game->player.pos_x,
                                      &game->player.pos_y,
                                      dx, dy);
                }

                if (world_check_encounter(&game->current_map,
                                          game->player.pos_x,
                                          game->player.pos_y)) {
                    int route_level = game->player.pos_y / 3 + 1;
                    game->wild_pokemon = battle_generate_wild(route_level);
                    game->state = STATE_BATTLE;
                }
            }
            break;

        case STATE_BATTLE:
            if (player_all_fainted(&game->player)) {
                game->state = STATE_GAME_OVER;
                break;
            }

            battle_run(&game->player, &game->wild_pokemon);

            if (player_all_fainted(&game->player)) {
                game->state = STATE_GAME_OVER;
            } else {
                game->state = STATE_EXPLORE;
            }
            break;

        default:
            game->state = STATE_GAME_OVER;
            break;
        }
    }

    ui_clear();
    ui_game_over();
}

void game_cleanup(Game *game)
{
    (void)game;
}
