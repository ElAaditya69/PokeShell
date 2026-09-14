#ifndef POKESHELL_UI_H
#define POKESHELL_UI_H

#include "pokemon.h"
#include "player.h"
#include "battle.h"

// clear the terminal screen
void ui_clear(void);

// print the title screen
void ui_print_title(void);

// print starter selection menu, return the chosen index (0-2)
int ui_starter_select(Pokemon starters[], int count);

// print the exploration map
void ui_print_map(int map[][20], int w, int h, int px, int py);

// print the battle screen with both pokemon
void ui_print_battle(const Pokemon *player_pokemon, const Pokemon *wild);

// print an HP bar for a pokemon
void ui_print_hp_bar(const Pokemon *pokemon);

// print the battle menu and get the player's choice
BattleAction ui_battle_menu(const Player *player);

// print a message to the screen
void ui_message(const char *msg);

// get a single keypress (no enter needed)
char ui_get_key(void);

// print game over screen
void ui_game_over(void);

// print victory screen
void ui_victory(void);

#endif
