#ifndef POKESHELL_BATTLE_H
#define POKESHELL_BATTLE_H

#include "pokemon.h"
#include "player.h"

// battle states
typedef enum {
    BATTLE_FIGHT,
    BATTLE_CATCH,
    BATTLE_SWITCH,
    BATTLE_RUN,
    BATTLE_WON,
    BATTLE_LOST,
    BATTLE_CAUGHT
} BattleAction;

// run a full battle between the player's current pokemon and a wild pokemon
// returns when the battle ends
void battle_run(Player *player, Pokemon *wild);

// calculate damage from attacker to defender using a move
int battle_calculate_damage(const Pokemon *attacker, const Pokemon *defender, const Move *move);

// try to catch a wild pokemon with a pokeball
// returns 1 if caught, 0 if not
int battle_try_catch(Pokemon *wild, int pokeball_count);

// the enemy ai picks a move (just picks the strongest one for now)
Move *battle_enemy_pick_move(Pokemon *enemy);

#endif
