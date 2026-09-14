#include "ui.h"
#include "pokemon.h"
#include "player.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/* ── terminal raw mode ────────────────────────────────────────── */

static struct termios orig_termios;
static int terminal_initialized = 0;

void ui_restore_terminal(void)
{
    if (terminal_initialized) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
        printf("\033[?25h");  /* show cursor */
        fflush(stdout);
        terminal_initialized = 0;
    }
}

void ui_init_terminal(void)
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(ui_restore_terminal);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    terminal_initialized = 1;

    printf("\033[?25l");  /* hide cursor */
    fflush(stdout);
}

/* ── basic helpers ────────────────────────────────────────────── */

void ui_clear(void)
{
    printf("\033[2J\033[H");
    fflush(stdout);
}

char ui_get_key(void)
{
    char c = 0;
    while (read(STDIN_FILENO, &c, 1) != 1) {}
    return c;
}

void ui_wait(void)
{
    printf("  Press any key to continue...\n");
    fflush(stdout);
    ui_get_key();
}

/* ── title screen ─────────────────────────────────────────────── */

void ui_print_title(void)
{
    ui_clear();
    printf("\n");
    printf("  \033[33m");
    printf("  ____   _    ____ ____  ___ ____ _   _ _____ ____ _  __\n");
    printf(" |  _ \\ / \\  / ___/ ___|| |_ _|  \\ | | ____/ ___| |/ /\n");
    printf(" | |_) / _ \\ \\___ \\___ \\  | ||  \\| |  _|| |   | ' / \n");
    printf(" |  __/ ___ \\ ___) |__) | | || |\\  | |__| |___| . \\ \n");
    printf(" |_| /_/   \\_\\____/____/ |___|_| \\_|_____\\____|_|\\_\\\n");
    printf("\033[0m");
    printf("\n");
    printf("  \033[90mTerminal Edition\033[0m\n\n");
    printf("  \033[97mPress any key...\033[0m\n");
    fflush(stdout);
    ui_get_key();
}

/* ── starter selection ────────────────────────────────────────── */

int ui_starter_select(void)
{
    Pokemon starters[3];
    starters[0] = pokemon_create_starter("Bulbasaur", TYPE_GRASS, 45, 49, 49);
    starters[1] = pokemon_create_starter("Charmander", TYPE_FIRE,  39, 52, 43);
    starters[2] = pokemon_create_starter("Squirtle",   TYPE_WATER, 44, 48, 65);
    int count = 3;

    int selected = 0;  /* 0-2 */

    while (1) {
        ui_clear();
        printf("  \033[97mChoose your starter Pokemon:\033[0m\n\n");

        for (int i = 0; i < count; i++) {
            if (i == selected)
                printf("  \033[32m> %d. %-10s  %s  HP:%d  ATK:%d  DEF:%d\033[0m\n",
                       i + 1, starters[i].name,
                       type_name(starters[i].type),
                       starters[i].max_hp,
                       starters[i].attack,
                       starters[i].defense);
            else
                printf("    %d. %-10s  %s  HP:%d  ATK:%d  DEF:%d\n",
                       i + 1, starters[i].name,
                       type_name(starters[i].type),
                       starters[i].max_hp,
                       starters[i].attack,
                       starters[i].defense);
        }

        printf("\n  W/S: navigate  D: confirm\n");
        fflush(stdout);

        char c = ui_get_key();
        if (c == 'w' || c == 'W') { selected--; if (selected < 0) selected = count - 1; }
        else if (c == 's' || c == 'S') { selected++; if (selected >= count) selected = 0; }
        else if (c == 'd' || c == 'D') return selected;
        else if (c >= '1' && c <= '3') return c - '1';
    }
}

/* ── HP bar ───────────────────────────────────────────────────── */

void ui_print_hp_bar(const Pokemon *pokemon)
{
    int bar_width = 20;
    int filled = (pokemon->hp * bar_width) / pokemon->max_hp;
    if (filled < 0) filled = 0;
    if (filled > bar_width) filled = bar_width;

    float ratio = (float)pokemon->hp / pokemon->max_hp;

    printf("  %s Lv.%d\n", pokemon->name, pokemon->level);
    printf("  HP: \033[97m[");
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) {
            if (ratio > 0.5f)       printf("\033[32m=");
            else if (ratio > 0.25f) printf("\033[33m=");
            else                    printf("\033[31m=");
        } else {
            printf("\033[90m-");
        }
    }
    printf("\033[97m] %d/%d\033[0m\n", pokemon->hp, pokemon->max_hp);
}

/* ── battle screen ────────────────────────────────────────────── */

void ui_print_battle(const Pokemon *player_pokemon, const Pokemon *wild)
{
    ui_clear();
    printf("\n");
    printf("  \033[31mWILD %s \033[90m(%s)\033[31m Lv.%d APPEARED!\033[0m\n\n",
           wild->name, type_name(wild->type), wild->level);
    ui_print_hp_bar(wild);
    printf("\n");
    ui_print_hp_bar(player_pokemon);
    printf("\n");
}

/* ── battle menu ──────────────────────────────────────────────── */

BattleAction ui_battle_menu(const Player *player, const Pokemon *active)
{
    printf("  What will \033[32m%s\033[0m do?\n", active->name);
    printf("  \033[32m> 1. Fight\033[0m\n");
    printf("    2. Catch (\033[33mPokeballs: %d\033[0m)\n", player->pokeballs);
    printf("    3. Run\n");
    printf("  ");
    fflush(stdout);

    char c = ui_get_key();
    switch (c) {
        case '1': return BATTLE_FIGHT;
        case '2': return BATTLE_CATCH;
        case '3': return BATTLE_RUN;
        default:  return BATTLE_FIGHT;
    }
}

/* ── move selection ───────────────────────────────────────────── */

int ui_move_select(const Pokemon *pokemon)
{
    int selected = 0;
    int total = pokemon->move_count + 1;  /* moves + back option */

    while (1) {
        printf("\033[2J\033[H");  /* clear for clean redraw */
        printf("  \033[97mChoose a move:\033[0m\n\n");

        for (int i = 0; i < pokemon->move_count; i++) {
            const Move *m = &pokemon->moves[i];
            if (i == selected)
                printf("  \033[32m> %d. %-12s  %s  PWR:%3d  PP:%d/%d\033[0m\n",
                       i + 1, m->name, type_name(m->type),
                       m->power, m->pp, m->max_pp);
            else
                printf("    %d. %-12s  %s  PWR:%3d  PP:%d/%d\n",
                       i + 1, m->name, type_name(m->type),
                       m->power, m->pp, m->max_pp);
        }

        if (selected == pokemon->move_count)
            printf("  \033[32m> 0. Back\033[0m\n");
        else
            printf("    0. Back\n");

        printf("\n  W/S: navigate  D: confirm  0: back\n");
        fflush(stdout);

        char c = ui_get_key();
        if (c == 'w' || c == 'W') { selected--; if (selected < 0) selected = total - 1; }
        else if (c == 's' || c == 'S') { selected++; if (selected >= total) selected = 0; }
        else if (c == '0') return -1;
        else if (c == 'd' || c == 'D') {
            if (selected == pokemon->move_count) return -1;
            return selected;
        }
        else if (c >= '1' && c <= '4') {
            int idx = c - '1';
            if (idx < pokemon->move_count) return idx;
        }
    }
}

/* ── message ──────────────────────────────────────────────────── */

void ui_message(const char *msg)
{
    printf("\n  %s\n", msg);
}

/* ── map viewport ─────────────────────────────────────────────── */

#define VIEWPORT_W 20
#define VIEWPORT_H 12

static const char *tile_color(TileType t)
{
    switch (t) {
        case TILE_GRASS:      return "\033[32m";    /* green */
        case TILE_WATER:      return "\033[34m";    /* blue */
        case TILE_TOWN:       return "\033[33m";    /* yellow */
        case TILE_CENTER:     return "\033[31m";    /* red */
        case TILE_GYM:        return "\033[35m";    /* magenta */
        case TILE_PATH:       return "\033[97m";    /* bright white */
        case TILE_WALL:       return "\033[90m";    /* dark gray */
        case TILE_WILD_GRASS: return "\033[92m";    /* bright green */
        default:              return "\033[90m";
    }
}

static const char *tile_char(TileType t)
{
    switch (t) {
        case TILE_GRASS:      return ".";
        case TILE_WATER:      return "~";
        case TILE_TOWN:       return "T";
        case TILE_CENTER:     return "C";
        case TILE_GYM:        return "G";
        case TILE_PATH:       return "-";
        case TILE_WALL:       return "#";
        case TILE_WILD_GRASS: return "W";
        default:              return "?";
    }
}

static const char *route_name(int y)
{
    if (y <= 6)  return "Pallet Town";
    if (y <= 12) return "Route 1";
    if (y <= 21) return "Route 2";
    return "Route 3";
}

void ui_print_map(const Map *map, const Player *player)
{
    int cam_x = player->pos_x - VIEWPORT_W / 2;
    int cam_y = player->pos_y - VIEWPORT_H / 2;

    /* clamp to map edges */
    if (cam_x < 0) cam_x = 0;
    if (cam_y < 0) cam_y = 0;
    if (cam_x + VIEWPORT_W > map->width)  cam_x = map->width  - VIEWPORT_W;
    if (cam_y + VIEWPORT_H > map->height) cam_y = map->height - VIEWPORT_H;

    /* header */
    printf("\n  \033[97m=== %s ===\033[0m\n\n", map->name);

    /* render viewport */
    for (int y = 0; y < VIEWPORT_H; y++) {
        printf("  ");
        for (int x = 0; x < VIEWPORT_W; x++) {
            int mx = cam_x + x;
            int my = cam_y + y;
            if (mx == player->pos_x && my == player->pos_y) {
                printf("\033[97m@\033[0m");
            } else {
                TileType t = map->tiles[my][mx];
                printf("%s%s\033[0m", tile_color(t), tile_char(t));
            }
        }
        printf("\n");
    }

    /* status bar */
    printf("\n");
    if (player->team_size > 0) {
        const Pokemon *lead = &player->team[0];
        printf("  \033[90mPOKESHELL\033[0m | "
               "\033[32m%s Lv.%d HP:%d/%d\033[0m | "
               "Pokeballs: %d | Badges: %d | "
               "\033[33m%s\033[0m\n",
               lead->name, lead->level, lead->hp, lead->max_hp,
               player->pokeballs, player->badges,
               route_name(player->pos_y));
    } else {
        printf("  \033[90mPOKESHELL\033[0m | "
               "Pokeballs: %d | Badges: %d | "
               "\033[33m%s\033[0m\n",
               player->pokeballs, player->badges,
               route_name(player->pos_y));
    }

    /* controls */
    printf("\n  \033[90mW/A/S/D: Move  T: Team  Q: Quit\033[0m\n");
}

/* ── team display ─────────────────────────────────────────────── */

void ui_show_team(const Player *player)
{
    ui_clear();
    printf("\n  \033[97m=== YOUR TEAM ===\033[0m\n\n");
    printf("  Badges: %d  |  Pokeballs: %d\n\n", player->badges, player->pokeballs);
    for (int i = 0; i < player->team_size; i++) {
        const Pokemon *p = &player->team[i];
        float ratio = (float)p->hp / p->max_hp;
        const char *hp_color;
        if (ratio > 0.5f)       hp_color = "\033[32m";
        else if (ratio > 0.25f) hp_color = "\033[33m";
        else                    hp_color = "\033[31m";

        printf("  %d. \033[32m%-10s\033[0m  %s  Lv.%2d  ",
               i + 1, p->name, type_name(p->type), p->level);
        printf("%sHP: %d/%d\033[0m\n", hp_color, p->hp, p->max_hp);
    }
    printf("\n  \033[97mPress any key to return...\033[0m\n");
    fflush(stdout);
    ui_get_key();
}

/* ── game over / victory ──────────────────────────────────────── */

void ui_game_over(void)
{
    ui_clear();
    printf("\n");
    printf("  \033[31m====================\033[0m\n");
    printf("  \033[31m    GAME  OVER\033[0m\n");
    printf("  \033[31m====================\033[0m\n\n");
    printf("  Better luck next time...\n\n");
}

void ui_victory(void)
{
    ui_clear();
    printf("\n");
    printf("  \033[33m====================\033[0m\n");
    printf("  \033[33m YOU ARE THE CHAMPION!\033[0m\n");
    printf("  \033[33m====================\033[0m\n\n");
    printf("  Congratulations! You did it!\n\n");
}
