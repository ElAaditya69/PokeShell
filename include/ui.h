#ifndef POKESHELL_UI_H
#define POKESHELL_UI_H

#include "pokemon.h"
#include "player.h"
#include "battle.h"
#include "world.h"

// initialize terminal for raw input (call at start of game)
void ui_init_terminal(void);

// restore terminal to original state (called automatically via atexit)
void ui_restore_terminal(void);

// clear the terminal screen
void ui_clear(void);

// print the title screen
void ui_print_title(void);

// print starter selection menu, return the chosen index (0-2)
int ui_starter_select(void);

// print the world map with player position
void ui_print_map(const Map *map, const Player *player);

// print the battle screen with both pokemon
void ui_print_battle(const Pokemon *player_pokemon, const Pokemon *wild);

// print an HP bar for a pokemon
void ui_print_hp_bar(const Pokemon *pokemon);

// print the battle menu and get the player's choice
BattleAction ui_battle_menu(const Player *player, const Pokemon *active);

// move selection submenu — returns index of chosen move, -1 for back
int ui_move_select(const Pokemon *pokemon);

// print a message to the screen
void ui_message(const char *msg);

// get a single keypress
char ui_get_key(void);

// wait for ENTER key
void ui_wait(void);

// print game over screen
void ui_game_over(void);

// print victory screen
void ui_victory(void);

// show the player's team (names, types, levels, HP) — waits for ENTER
void ui_show_team(const Player *player);

#endif
