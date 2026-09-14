#ifndef POKESHELL_POKEMON_H
#define POKESHELL_POKEMON_H

#include "types.h"
#include "move.h"

#define MAX_MOVES 4

// a pokemon instance (the one you caught, not the species)
typedef struct {
    char name[32];
    Type type;
    int level;
    int hp;
    int max_hp;
    int attack;
    int defense;
    int xp;
    int xp_to_next;
    Move moves[MAX_MOVES];
    int move_count;
} Pokemon;

// species base stats (loaded from data/pokemon.txt)
typedef struct {
    char name[32];
    Type type;
    int base_hp;
    int base_atk;
    int base_def;
} PokemonSpecies;

// create a pokemon from species data at a given level
Pokemon pokemon_create(const char *name, Type type, int level);

// create a starter pokemon at level 5 with correct base stats and starting moves
Pokemon pokemon_create_starter(const char *name, Type type, int base_hp, int base_atk, int base_def);

// calculate a stat based on base stat and level
int pokemon_calc_stat(int base, int level);

// gain xp and check for level up
void pokemon_gain_xp(Pokemon *pokemon, int xp);

// heal a pokemon to full
void pokemon_heal(Pokemon *pokemon);

// check if a pokemon has fainted
int pokemon_is_fainted(const Pokemon *pokemon);

#endif
