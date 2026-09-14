#include "battle.h"
#include "ui.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void battle_execute_move(Pokemon *attacker, Pokemon *defender, Move *move)
{
    if (move->pp <= 0) return;
    move->pp--;

    // accuracy check
    int roll = rand() % 100;
    if (roll >= move->accuracy) {
        printf("  %s used %s... but it missed!\n", attacker->name, move->name);
        return;
    }

    int damage = battle_calculate_damage(attacker, defender, move);

    if (damage == 0) {
        printf("  %s used %s... it had no effect!\n", attacker->name, move->name);
        return;
    }

    defender->hp -= damage;
    if (defender->hp < 0) defender->hp = 0;

    printf("  %s used %s! Dealt %d damage!\n", attacker->name, move->name, damage);
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
                battle_execute_move(player_pokemon, wild, &player_pokemon->moves[move_idx]);
            } else {
                continue;  // back pressed, re-show battle menu
            }
        } else if (action == BATTLE_CATCH) {
            if (player->pokeballs <= 0) {
                printf("  No Pokeballs left!\n");
                ui_wait();
                continue;
            }
            player->pokeballs--;
            caught = battle_try_catch(wild, player->pokeballs + 1);
            if (caught) {
                printf("  You caught %s!\n", wild->name);
                player_add_pokemon(player, *wild);
            } else {
                printf("  Oh no! %s broke free!\n", wild->name);
            }
            ui_wait();
        } else if (action == BATTLE_RUN) {
            if (rand() % 2 == 0) {
                escaped = 1;
                printf("  Got away safely!\n");
                ui_wait();
            } else {
                printf("  Couldn't get away!\n");
                ui_wait();
            }
        }

        // enemy turn if battle isn't over
        if (!pokemon_is_fainted(player_pokemon) && !pokemon_is_fainted(wild) && !escaped && !caught) {
            Move *enemy_move = battle_enemy_pick_move(wild);
            battle_execute_move(wild, player_pokemon, enemy_move);
            ui_wait();
        }
    }

    if (caught) return;

    if (pokemon_is_fainted(wild)) {
        int xp = player_pokemon->level * 15;
        printf("\n  %s fainted! Gained %d XP!\n", wild->name, xp);
        pokemon_gain_xp(player_pokemon, xp);
        ui_wait();
    } else if (pokemon_is_fainted(player_pokemon)) {
        printf("\n  %s fainted!\n", player_pokemon->name);
        ui_wait();
    }
}

int battle_calculate_damage(const Pokemon *attacker, const Pokemon *defender, const Move *move)
{
    if (move->power == 0) return 0;  // status move

    // damage formula: ((2*level/5+2) * power * atk/def) / 50 + 2
    float level_mod = (2.0f * attacker->level / 5.0f) + 2.0f;
    float damage = (level_mod * move->power * attacker->attack) / defender->defense;
    damage = damage / 50.0f + 2.0f;

    // type effectiveness
    float eff = type_effectiveness(move->type, defender->type);
    damage *= eff;

    // random factor between 0.85 and 1.0
    float random = (float)(rand() % 16 + 85) / 100.0f;
    damage *= random;

    int result = (int)damage;
    if (result < 1) result = 1;

    return result;
}

int battle_try_catch(Pokemon *wild, int pokeball_count)
{
    if (pokeball_count <= 0) return 0;

    // simple catch formula based on remaining hp
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
    // pick a random move with pp > 0
    int valid[MAX_MOVES];
    int count = 0;
    for (int i = 0; i < enemy->move_count; i++) {
        if (enemy->moves[i].pp > 0) {
            valid[count++] = i;
        }
    }
    if (count == 0) {
        // no pp left, use first move anyway (struggle)
        return &enemy->moves[0];
    }
    return &enemy->moves[valid[rand() % count]];
}

Pokemon battle_generate_wild(int route_level)
{
    Type wild_types[] = {TYPE_FIRE, TYPE_GRASS, TYPE_WATER};
    const char *wild_names[] = {"Charmander", "Bulbasaur", "Squirtle"};
    int type_idx = rand() % 3;
    int level = route_level + rand() % 3;
    return pokemon_create(wild_names[type_idx], wild_types[type_idx], level);
}
