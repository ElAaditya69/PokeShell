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

// type color code string (ANSI escape) for the given Type
const char *ui_type_color(Type type);

// type label string for the given Type
const char *ui_type_label(Type type);

// print a colored HP bar standalone
void ui_hp_bar(const Pokemon *pokemon);

// encounter animation — "A wild X appeared!"
void ui_encounter_animation(const Pokemon *wild);

// move result — attacker used move, damage, effectiveness
void ui_move_result(const char *attacker, const char *move_name, int damage, float effectiveness);

// catch success animation
void ui_catch_success(const Pokemon *pokemon);

// catch failure message
void ui_catch_failure(const Pokemon *pokemon);

// run attempt result
void ui_run_result(int success);

// pokemon fainted message (xp=0 if wild didn't give xp yet, is_wild=1 for wild, 0 for player)
void ui_fainted(const char *name, int xp, int is_wild);

// pokemon center healing animation
void ui_pokemon_center(void);

// level up with stat increases
void ui_level_up(const Pokemon *pokemon, int old_level, int old_atk, int old_def, int old_hp);

// gym leader intro
void ui_gym_leader_intro(const char *name);

// gym leader sends pokemon
void ui_gym_leader_sends(const Pokemon *pokemon);

// got gym badge
void ui_gym_badge(void);

// pokeball throw animation
void ui_catch_animation(const Pokemon *pokemon);

// xp gained message
void ui_xp_gain(int xp);

// no pokeballs message
void ui_no_pokeballs(void);

// team full message
void ui_team_full(void);

// starter chosen message
void ui_chosen(const char *name);

#endif
