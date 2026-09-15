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
    /* start inside the town */
    game->player.pos_x = 2;
    game->player.pos_y = 2;
}

void game_run(Game *game)
{
    srand((unsigned int)time(NULL));
    ui_init_terminal();
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

            /* starter data: name, type, base_hp, base_atk, base_def */
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
            ui_chosen(starter.name);
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

                /* team display */
                if (input == 't' || input == 'T') {
                    ui_show_team(&game->player);
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

                /* check current tile for special interactions */
                TileType tile = world_get_tile(&game->current_map,
                                               game->player.pos_x,
                                               game->player.pos_y);

                if (tile == TILE_CENTER) {
                    player_heal_team(&game->player);
                    ui_pokemon_center();
                    ui_wait();
                    break;  /* no encounter on center tile */
                }

                if (tile == TILE_GYM && !game->current_map.gym_defeated) {
                    /* set up gym leader battle */
                    game->is_gym_battle = 1;
                    game->gym_team_size = GYM_TEAM_SIZE;
                    game->gym_current = 0;
                    game->gym_team[0] = pokemon_create_starter("Squirtle",   TYPE_WATER, 44, 48, 65);
                    game->gym_team[1] = pokemon_create_starter("Bulbasaur",  TYPE_GRASS, 45, 49, 49);
                    game->gym_team[2] = pokemon_create_starter("Charmander", TYPE_FIRE,  39, 52, 43);
                    /* boost gym pokemon levels */
                    for (int i = 0; i < GYM_TEAM_SIZE; i++) {
                        while (game->gym_team[i].level < 12 + i) {
                            pokemon_gain_xp(&game->gym_team[i],
                                            game->gym_team[i].xp_to_next);
                        }
                    }
                    /* set wild_pokemon to first gym pokemon */
                    game->wild_pokemon = game->gym_team[0];
                    ui_gym_leader_intro("Brock");
                    ui_wait();
                    game->state = STATE_BATTLE;
                    break;
                }

                /* encounter check for grass and wild grass */
                if (world_check_encounter(&game->current_map,
                                          game->player.pos_x,
                                          game->player.pos_y)) {
                    /* route level based on Y position */
                    int route_level;
                    if (game->player.pos_y <= 6)
                        route_level = 3;   /* town — shouldn't encounter, but fallback */
                    else if (game->player.pos_y <= 12)
                        route_level = 3 + (game->player.pos_y - 7) / 2;  /* Route 1: 3-6 */
                    else
                        route_level = 8 + (game->player.pos_y - 13) / 3; /* Route 2: 8-12 */

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
                break;
            }

            /* gym battle: advance to next pokemon if any remain */
            if (game->is_gym_battle) {
                game->gym_current++;
                if (game->gym_current < game->gym_team_size) {
                    /* next gym pokemon still alive? check */
                    if (!pokemon_is_fainted(&game->gym_team[game->gym_current])) {
                        game->wild_pokemon = game->gym_team[game->gym_current];
                        ui_gym_leader_sends(&game->gym_team[game->gym_current]);
                        ui_wait();
                        game->state = STATE_BATTLE;
                        break;
                    }
                    /* skip fainted ones */
                    while (game->gym_current < game->gym_team_size &&
                           pokemon_is_fainted(&game->gym_team[game->gym_current])) {
                        game->gym_current++;
                    }
                    if (game->gym_current < game->gym_team_size) {
                        game->wild_pokemon = game->gym_team[game->gym_current];
                        ui_gym_leader_sends(&game->gym_team[game->gym_current]);
                        ui_wait();
                        game->state = STATE_BATTLE;
                        break;
                    }
                }
                /* all gym pokemon defeated */
                game->current_map.gym_defeated = 1;
                game->player.badges++;
                game->is_gym_battle = 0;
                ui_gym_badge();
                ui_wait();
            }

            game->state = STATE_EXPLORE;
            break;

        default:
            game->state = STATE_GAME_OVER;
            break;
        }
    }

    ui_clear();
    ui_game_over();
    ui_wait();
    ui_restore_terminal();
}

void game_cleanup(Game *game)
{
    (void)game;
}
