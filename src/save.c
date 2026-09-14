#include "save.h"
#include <stdio.h>
#include <string.h>

int save_game(const Game *game, const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f) return 0;

    fprintf(f, "PLAYER_NAME %s\n", game->player.name);
    fprintf(f, "POKEBALLS %d\n", game->player.pokeballs);
    fprintf(f, "BADGES %d\n", game->player.badges);
    fprintf(f, "POSITION %d %d\n", game->player.pos_x, game->player.pos_y);
    fprintf(f, "TEAM_SIZE %d\n", game->player.team_size);

    for (int i = 0; i < game->player.team_size; i++) {
        const Pokemon *p = &game->player.team[i];
        fprintf(f, "POKEMON %s %s %d %d %d %d %d %d\n",
                p->name, type_name(p->type), p->level,
                p->hp, p->max_hp, p->attack, p->defense, p->xp);
    }

    fprintf(f, "MAP %s\n", game->current_map.name);

    fclose(f);
    return 1;
}

int load_game(Game *game, const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f) return 0;

    // TODO: parse the save file properly
    // for now just return success if file exists
    (void)game;

    fclose(f);
    return 1;
}
