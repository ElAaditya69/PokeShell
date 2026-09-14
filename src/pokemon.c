#include "pokemon.h"
#include <string.h>
#include <stdio.h>

Pokemon pokemon_create(const char *name, Type type, int level)
{
    Pokemon p;
    memset(&p, 0, sizeof(Pokemon));

    strncpy(p.name, name, sizeof(p.name) - 1);
    p.type = type;
    p.level = level;
    p.max_hp = pokemon_calc_stat(45, level);  // default base hp
    p.hp = p.max_hp;
    p.attack = pokemon_calc_stat(49, level);
    p.defense = pokemon_calc_stat(49, level);
    p.xp = 0;
    p.xp_to_next = level * level * 5;  // simple xp curve
    p.move_count = 0;

    return p;
}

int pokemon_calc_stat(int base, int level)
{
    // simple stat formula
    return ((2 * base * level) / 100) + level + 5;
}

void pokemon_gain_xp(Pokemon *pokemon, int xp)
{
    pokemon->xp += xp;
    while (pokemon->xp >= pokemon->xp_to_next) {
        pokemon->xp -= pokemon->xp_to_next;
        pokemon->level++;
        // recalc stats
        int old_max_hp = pokemon->max_hp;
        pokemon->max_hp = pokemon_calc_stat(45, pokemon->level);
        pokemon->hp += (pokemon->max_hp - old_max_hp);  // gain the extra hp
        pokemon->attack = pokemon_calc_stat(49, pokemon->level);
        pokemon->defense = pokemon_calc_stat(49, pokemon->level);
        pokemon->xp_to_next = pokemon->level * pokemon->level * 5;
        printf("%s leveled up to %d!\n", pokemon->name, pokemon->level);
    }
}

void pokemon_heal(Pokemon *pokemon)
{
    pokemon->hp = pokemon->max_hp;
}

int pokemon_is_fainted(const Pokemon *pokemon)
{
    return pokemon->hp <= 0;
}
