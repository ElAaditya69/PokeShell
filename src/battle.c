#include "battle.h"
#include "ui.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static BattleMoveResult battle_execute_move(Pokemon *attacker, Pokemon *defender, Move *move)
{
    BattleMoveResult result;
    memset(&result, 0, sizeof(result));
    strncpy(result.attacker, attacker->name, 31);
    strncpy(result.defender, defender->name, 31);
    strncpy(result.move_name, move->name, 31);

    if (move->pp <= 0) {
        result.missed = 1;
        result.effectiveness = 0.0f;
        return result;
    }
    move->pp--;

    /* accuracy check */
    int roll = rand() % 100;
    if (roll >= move->accuracy) {
        result.missed = 1;
        result.effectiveness = 0.0f;
        return result;
    }

    int damage = battle_calculate_damage(attacker, defender, move);

    if (damage == 0) {
        result.missed = 1;
        result.effectiveness = 0.0f;
        return result;
    }

    defender->hp -= damage;
    if (defender->hp < 0) defender->hp = 0;

    result.damage = damage;
    result.effectiveness = type_effectiveness(move->type, defender->type);
    result.missed = 0;
    return result;
}

void battle_run(Player *player, Pokemon *wild)
{
    Pokemon *player_pokemon = &player->team[0];
    int escaped = 0;
    int caught = 0;

    while (!pokemon_is_fainted(player_pokemon) && !pokemon_is_fainted(wild) && !escaped && !caught) {
        ui_print_battle(player_pokemon, wild);
        BattleAction action = ui_battle_menu(player, player_pokemon);

        if (action == BATTLE_FIGHT) {
            int move_idx = ui_move_select(player_pokemon);
            if (move_idx >= 0 && move_idx < player_pokemon->move_count) {
                BattleMoveResult r = battle_execute_move(player_pokemon, wild,
                                                         &player_pokemon->moves[move_idx]);
                ui_move_result(r.attacker, r.move_name, r.damage, r.effectiveness);
                ui_wait();
            } else {
                continue;  /* back pressed, re-show battle menu */
            }
        } else if (action == BATTLE_CATCH) {
            if (player->pokeballs <= 0) {
                ui_no_pokeballs();
                ui_wait();
                continue;
            }
            if (player->team_size >= MAX_TEAM) {
                ui_team_full();
                ui_wait();
                continue;
            }
            player->pokeballs--;

            /* catch animation */
            ui_catch_animation(wild);

            caught = battle_try_catch(wild, player->pokeballs + 1);
            if (caught) {
                ui_catch_success(wild);
                player_add_pokemon(player, *wild);
            } else {
                ui_catch_failure(wild);
            }
            ui_wait();
        } else if (action == BATTLE_RUN) {
            if (rand() % 2 == 0) {
                escaped = 1;
                ui_run_result(1);
                ui_wait();
            } else {
                ui_run_result(0);
                ui_wait();
            }
        }

        /* enemy turn if battle isn't over */
        if (!pokemon_is_fainted(player_pokemon) && !pokemon_is_fainted(wild) && !escaped && !caught) {
            Move *enemy_move = battle_enemy_pick_move(wild);
            if (enemy_move) {
                BattleMoveResult r = battle_execute_move(wild, player_pokemon, enemy_move);
                ui_move_result(r.attacker, r.move_name, r.damage, r.effectiveness);
            }
            ui_wait();
        }
    }

    if (caught) return;

    if (pokemon_is_fainted(wild)) {
        int xp = player_pokemon->level * 15;
        ui_fainted(wild->name, xp, 1);
        pokemon_gain_xp(player_pokemon, xp);
        ui_wait();
    } else if (pokemon_is_fainted(player_pokemon)) {
        ui_fainted(player_pokemon->name, 0, 0);
        ui_wait();
    }
}

int battle_calculate_damage(const Pokemon *attacker, const Pokemon *defender, const Move *move)
{
    if (move->power == 0) return 0;  /* status move */

    /* damage formula: ((2*level/5+2) * power * atk/def) / 50 + 2 */
    float level_mod = (2.0f * attacker->level / 5.0f) + 2.0f;
    float damage = (level_mod * move->power * attacker->attack) / defender->defense;
    damage = damage / 50.0f + 2.0f;

    /* type effectiveness */
    float eff = type_effectiveness(move->type, defender->type);
    damage *= eff;

    /* random factor between 0.85 and 1.0 */
    float random = (float)(rand() % 16 + 85) / 100.0f;
    damage *= random;

    int result = (int)damage;
    if (result < 1) result = 1;

    return result;
}

int battle_try_catch(Pokemon *wild, int pokeball_count)
{
    if (pokeball_count <= 0) return 0;

    /* simple catch formula based on remaining hp */
    float hp_ratio = (float)wild->hp / wild->max_hp;
    int catch_chance = (int)((1.0f - hp_ratio) * 100);
    if (catch_chance < 10) catch_chance = 10;
    if (catch_chance > 95) catch_chance = 95;

    int roll = rand() % 100;
    return roll < catch_chance;
}

Move *battle_enemy_pick_move(Pokemon *enemy)
{
    if (enemy->move_count == 0) return NULL;
    /* pick a random move with pp > 0 */
    int valid[MAX_MOVES];
    int count = 0;
    for (int i = 0; i < enemy->move_count; i++) {
        if (enemy->moves[i].pp > 0) {
            valid[count++] = i;
        }
    }
    if (count == 0) {
        /* no pp left, use first move anyway (struggle) */
        return &enemy->moves[0];
    }
    return &enemy->moves[valid[rand() % count]];
}

Pokemon battle_generate_wild(int route_level)
{
    /* Route 1 species (level 3-6) */
    typedef struct { const char *name; Type type; int base_hp; int base_atk; int base_def; } Species;

    const Species route1[] = {
        {"Pidgey",   TYPE_NORMAL, 40, 45, 40},
        {"Rattata",  TYPE_NORMAL, 30, 56, 35},
        {"Caterpie", TYPE_GRASS,  45, 30, 35},
    };
    const Species route2[] = {
        {"Pikachu", TYPE_ELECTRIC, 35, 55, 40},
        {"Eevee",   TYPE_NORMAL,   55, 55, 50},
        {"Oddish",  TYPE_GRASS,    45, 50, 55},
    };
    const Species gym_route[] = {
        {"Gastly",  TYPE_NORMAL, 30, 35, 30},
        {"Machop",  TYPE_NORMAL, 70, 80, 50},
        {"Zubat",   TYPE_NORMAL, 40, 45, 35},
    };

    const Species *pool;
    int pool_size;

    if (route_level <= 6) {
        pool = route1;
        pool_size = 3;
    } else if (route_level <= 12) {
        pool = route2;
        pool_size = 3;
    } else {
        pool = gym_route;
        pool_size = 3;
    }

    int idx = rand() % pool_size;
    int level = route_level + rand() % 3;
    if (level < 3) level = 3;
    if (level > 20) level = 20;

    return pokemon_create_wild(pool[idx].name, pool[idx].type,
                               pool[idx].base_hp, pool[idx].base_atk,
                               pool[idx].base_def, level);
}
