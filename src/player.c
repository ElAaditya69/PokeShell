#include "player.h"
#include <string.h>

Player player_create(const char *name)
{
    Player p;
    memset(&p, 0, sizeof(Player));

    strncpy(p.name, name, sizeof(p.name) - 1);
    p.team_size = 0;
    p.pokeballs = START_POKEBALLS;
    p.badges = 0;
    p.pos_x = 0;
    p.pos_y = 0;

    return p;
}

int player_add_pokemon(Player *player, Pokemon pokemon)
{
    if (player->team_size >= MAX_TEAM) return 0;
    player->team[player->team_size] = pokemon;
    player->team_size++;
    return 1;
}

void player_remove_pokemon(Player *player, int index)
{
    if (index < 0 || index >= player->team_size) return;
    for (int i = index; i < player->team_size - 1; i++) {
        player->team[i] = player->team[i + 1];
    }
    player->team_size--;
}

int player_use_pokeball(Player *player)
{
    if (player->pokeballs <= 0) return 0;
    player->pokeballs--;
    return 1;
}

int player_all_fainted(const Player *player)
{
    for (int i = 0; i < player->team_size; i++) {
        if (!pokemon_is_fainted(&player->team[i])) return 0;
    }
    return 1;
}

void player_heal_team(Player *player)
{
    for (int i = 0; i < player->team_size; i++) {
        pokemon_heal(&player->team[i]);
    }
}
