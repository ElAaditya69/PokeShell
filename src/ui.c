#include "ui.h"
#include "pokemon.h"
#include "player.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ui_clear(void)
{
    printf("\033[2J\033[H");
    fflush(stdout);
}

void ui_print_title(void)
{
    ui_clear();
    printf("\n");
    printf("  ____   _    ____ ____  ___ ____ _   _ _____ ____ _  __\n");
    printf(" |  _ \\ / \\  / ___/ ___|| |_ _|  \\ | | ____/ ___| |/ /\n");
    printf(" | |_) / _ \\ \\___ \\___ \\  | ||  \\| |  _|| |   | ' / \n");
    printf(" |  __/ ___ \\ ___) |__) | | || |\\  | |__| |___| . \\ \n");
    printf(" |_| /_/   \\_\\____/____/ |___|_| \\_|_____\\____|_|\\_\\\n");
    printf("\n");
    printf("  Terminal Edition\n\n");
    printf("  Press ENTER to start...\n");
    getchar();
}

int ui_starter_select(void)
{
    Pokemon starters[3];
    starters[0] = pokemon_create_starter("Bulbasaur", TYPE_GRASS, 45, 49, 49);
    starters[1] = pokemon_create_starter("Charmander", TYPE_FIRE,  39, 52, 43);
    starters[2] = pokemon_create_starter("Squirtle",   TYPE_WATER, 44, 48, 65);
    int count = 3;

    ui_clear();
    printf("  Choose your starter Pokemon:\n\n");
    for (int i = 0; i < count; i++) {
        printf("  %d. %s (%s)  HP:%d  ATK:%d  DEF:%d\n",
               i + 1, starters[i].name,
               type_name(starters[i].type),
               starters[i].max_hp,
               starters[i].attack,
               starters[i].defense);
    }
    printf("\n  Enter choice (1-%d): ", count);

    int choice = 0;
    scanf("%d", &choice);
    while (getchar() != '\n');  // clear input buffer

    if (choice < 1 || choice > count) choice = 1;
    return choice - 1;
}

void ui_print_hp_bar(const Pokemon *pokemon)
{
    int bar_width = 20;
    int filled = (pokemon->hp * bar_width) / pokemon->max_hp;
    if (filled < 0) filled = 0;
    if (filled > bar_width) filled = bar_width;

    printf("  %s Lv.%d\n", pokemon->name, pokemon->level);
    printf("  HP: [");
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) printf("=");
        else printf("-");
    }
    printf("] %d/%d\n", pokemon->hp, pokemon->max_hp);
}

void ui_print_battle(const Pokemon *player_pokemon, const Pokemon *wild)
{
    ui_clear();
    printf("\n  WILD %s (%s) Lv.%d APPEARED!\n\n",
           wild->name, type_name(wild->type), wild->level);
    ui_print_hp_bar(wild);
    printf("\n");
    ui_print_hp_bar(player_pokemon);
    printf("\n");
}

BattleAction ui_battle_menu(const Player *player, const Pokemon *active)
{
    printf("  What will %s do?\n", active->name);
    printf("  1. Fight\n");
    printf("  2. Catch (Pokeballs: %d)\n", player->pokeballs);
    printf("  3. Run\n");
    printf("  > ");

    int choice = 0;
    scanf("%d", &choice);
    while (getchar() != '\n');

    switch (choice) {
        case 1: return BATTLE_FIGHT;
        case 2: return BATTLE_CATCH;
        case 3: return BATTLE_RUN;
        default: return BATTLE_FIGHT;
    }
}

int ui_move_select(const Pokemon *pokemon)
{
    printf("  Choose a move:\n");
    for (int i = 0; i < pokemon->move_count; i++) {
        const Move *m = &pokemon->moves[i];
        printf("  %d. %-12s  TYPE:%-8s  PWR:%3d  PP:%d/%d\n",
               i + 1, m->name, type_name(m->type),
               m->power, m->pp, m->max_pp);
    }
    printf("  0. Back\n");
    printf("  > ");

    int choice = 0;
    scanf("%d", &choice);
    while (getchar() != '\n');

    if (choice == 0) return -1;
    if (choice < 1 || choice > pokemon->move_count) return -1;
    return choice - 1;
}

void ui_message(const char *msg)
{
    printf("\n  %s\n", msg);
}

char ui_get_key(void)
{
    char c;
    scanf(" %c", &c);
    while (getchar() != '\n');
    return c;
}

void ui_wait(void)
{
    printf("  Press ENTER to continue...");
    getchar();
}

void ui_print_map(const Map *map, const Player *player)
{
    printf("\n  === %s ===\n\n", map->name);

    for (int y = 0; y < map->height; y++) {
        printf("  ");
        for (int x = 0; x < map->width; x++) {
            if (x == player->pos_x && y == player->pos_y) {
                printf("@");
            } else {
                switch (map->tiles[y][x]) {
                    case TILE_GRASS:      printf(".");  break;
                    case TILE_WATER:      printf("~");  break;
                    case TILE_TOWN:       printf("T");  break;
                    case TILE_PATH:       printf("-");  break;
                    case TILE_WALL:       printf("#");  break;
                    case TILE_CENTER:     printf("C");  break;
                    case TILE_GYM:        printf("G");  break;
                    case TILE_WILD_GRASS: printf("W");  break;
                    default:              printf("?");  break;
                }
            }
        }
        printf("\n");
    }

    printf("\n  @ = You  . = Grass  ~ = Water  T = Town  # = Wall\n");
    printf("  C = Center  G = Gym  W = Wild Grass  - = Path\n");
    printf("  Move: W/A/S/D  |  Team: T  |  Quit: Q\n\n");
}

void ui_game_over(void)
{
    ui_clear();
    printf("\n");
    printf("  ====================\n");
    printf("     GAME  OVER\n");
    printf("  ====================\n\n");
    printf("  Better luck next time...\n\n");
}

void ui_victory(void)
{
    ui_clear();
    printf("\n");
    printf("  ====================\n");
    printf("   YOU ARE THE CHAMPION!\n");
    printf("  ====================\n\n");
    printf("  Congratulations! You did it!\n\n");
}

void ui_show_team(const Player *player)
{
    ui_clear();
    printf("\n  === YOUR TEAM ===\n\n");
    printf("  Badges: %d  |  Pokeballs: %d\n\n", player->badges, player->pokeballs);
    for (int i = 0; i < player->team_size; i++) {
        const Pokemon *p = &player->team[i];
        printf("  %d. %-10s  %s  Lv.%2d  HP: %d/%d\n",
               i + 1, p->name, type_name(p->type), p->level,
               p->hp, p->max_hp);
    }
    printf("\n  Press ENTER to return...\n");
    getchar();
}
