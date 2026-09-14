#include "battle.h"
#include "ui.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void battle_run(Player *player, Pokemon *wild)
{
    // TODO: implement the full battle loop
    (void)player;
    (void)wild;
    printf("Battle system not yet implemented.\n");
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
    // just pick the first move with pp for now
    for (int i = 0; i < enemy->move_count; i++) {
        if (enemy->moves[i].pp > 0) {
            return &enemy->moves[i];
        }
    }
    return &enemy->moves[0];
}
