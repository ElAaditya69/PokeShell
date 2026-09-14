#include "pokemon.h"
#include "move.h"
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

Pokemon pokemon_create_starter(const char *name, Type type, int base_hp, int base_atk, int base_def)
{
    Pokemon p;
    memset(&p, 0, sizeof(Pokemon));

    int level = 5;

    strncpy(p.name, name, sizeof(p.name) - 1);
    p.type = type;
    p.level = level;
    p.max_hp = pokemon_calc_stat(base_hp, level);
    p.hp = p.max_hp;
    p.attack = pokemon_calc_stat(base_atk, level);
    p.defense = pokemon_calc_stat(base_def, level);
    p.xp = 0;
    p.xp_to_next = level * level * 5;
    p.move_count = 0;

    // assign starting moves
    move_get_starter_moves(name, p.moves, &p.move_count);

    return p;
}

Pokemon pokemon_create_wild(const char *name, Type type, int base_hp, int base_atk, int base_def, int level)
{
    Pokemon p;
    memset(&p, 0, sizeof(Pokemon));

    strncpy(p.name, name, sizeof(p.name) - 1);
    p.type = type;
    p.level = level;
    p.max_hp = pokemon_calc_stat(base_hp, level);
    p.hp = p.max_hp;
    p.attack = pokemon_calc_stat(base_atk, level);
    p.defense = pokemon_calc_stat(base_def, level);
    p.xp = 0;
    p.xp_to_next = level * level * 5;
    p.move_count = 0;

    /* assign species-specific moves */
    if (strcmp(name, "Pidgey") == 0) {
        p.moves[0] = move_create("Gust", TYPE_NORMAL, 40, 100, 35);
        p.moves[1] = move_create("Tackle", TYPE_NORMAL, 40, 100, 35);
        p.move_count = 2;
    } else if (strcmp(name, "Rattata") == 0) {
        p.moves[0] = move_create("Tackle", TYPE_NORMAL, 40, 100, 35);
        p.moves[1] = move_create("Bite", TYPE_NORMAL, 60, 100, 25);
        p.move_count = 2;
    } else if (strcmp(name, "Caterpie") == 0) {
        p.moves[0] = move_create("Tackle", TYPE_NORMAL, 40, 100, 35);
        p.move_count = 1;
    } else if (strcmp(name, "Pikachu") == 0) {
        p.moves[0] = move_create("ThunderShock", TYPE_ELECTRIC, 40, 100, 30);
        p.moves[1] = move_create("QuickAttack", TYPE_NORMAL, 40, 100, 20);
        p.move_count = 2;
    } else if (strcmp(name, "Eevee") == 0) {
        p.moves[0] = move_create("Tackle", TYPE_NORMAL, 40, 100, 35);
        p.moves[1] = move_create("Bite", TYPE_NORMAL, 60, 100, 25);
        p.move_count = 2;
    } else if (strcmp(name, "Oddish") == 0) {
        p.moves[0] = move_create("VineWhip", TYPE_GRASS, 45, 100, 25);
        p.moves[1] = move_create("Absorb", TYPE_GRASS, 40, 100, 25);
        p.move_count = 2;
    } else if (strcmp(name, "Gastly") == 0) {
        p.moves[0] = move_create("Lick", TYPE_NORMAL, 30, 100, 30);
        p.moves[1] = move_create("ShadowBall", TYPE_NORMAL, 80, 100, 10);
        p.move_count = 2;
    } else if (strcmp(name, "Machop") == 0) {
        p.moves[0] = move_create("LowKick", TYPE_NORMAL, 50, 90, 20);
        p.moves[1] = move_create("KarateChop", TYPE_NORMAL, 50, 100, 25);
        p.move_count = 2;
    } else if (strcmp(name, "Zubat") == 0) {
        p.moves[0] = move_create("Bite", TYPE_NORMAL, 60, 100, 25);
        p.moves[1] = move_create("WingAttack", TYPE_NORMAL, 60, 100, 20);
        p.move_count = 2;
    }

    return p;
}
