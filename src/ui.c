#include "ui.h"
#include "pokemon.h"
#include "player.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/* ═══════════════════════════════════════════════════════════════════
 *  Box-drawing constants and helpers
 * ═══════════════════════════════════════════════════════════════════ */

#define BOX_W 52  /* total width including border chars */
#define IN_W  50  /* interior width (BOX_W - 2) */

/* ── terminal raw mode ────────────────────────────────────────── */

static struct termios orig_termios;

void ui_init_terminal(void)
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void ui_restore_terminal(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

char ui_get_key(void)
{
    char c = 0;
    read(STDIN_FILENO, &c, 1);
    return c;
}

void ui_wait(void)
{
    printf("  Press ENTER to continue...");
    fflush(stdout);
    while (ui_get_key() != '\n')
        ;
}

void ui_clear(void)
{
    printf("\033[2J\033[H");
    fflush(stdout);
}

/* ═══════════════════════════════════════════════════════════════════
 *  ANSI color definitions
 * ═══════════════════════════════════════════════════════════════════ */

#define C_RESET   "\033[0m"
#define C_RED     "\033[31m"
#define C_GREEN   "\033[32m"
#define C_YELLOW  "\033[33m"
#define C_BLUE    "\033[34m"
#define C_MAGENTA "\033[35m"
#define C_CYAN    "\033[36m"
#define C_WHITE   "\033[37m"
#define C_BOLD    "\033[1m"
#define C_DIM     "\033[2m"

#define C_BG_RED     "\033[41m"
#define C_BG_GREEN   "\033[42m"
#define C_BG_YELLOW  "\033[43m"
#define C_BG_BLUE    "\033[44m"
#define C_BG_MAGENTA "\033[45m"
#define C_BG_CYAN    "\033[46m"
#define C_BG_WHITE   "\033[47m"

/* ═══════════════════════════════════════════════════════════════════
 *  Type color mapping (FireRed-style palette)
 * ═══════════════════════════════════════════════════════════════════ */

const char *ui_type_color(Type type)
{
    switch (type) {
        case TYPE_NORMAL:   return C_WHITE;
        case TYPE_FIRE:     return "\033[38;5;208m"; /* orange */
        case TYPE_WATER:    return C_BLUE;
        case TYPE_GRASS:    return C_GREEN;
        case TYPE_ELECTRIC: return C_YELLOW;
        case TYPE_ICE:      return C_CYAN;
        default:            return C_WHITE;
    }
}

static const char *ui_type_bg(Type type)
{
    switch (type) {
        case TYPE_NORMAL:   return C_BG_WHITE;
        case TYPE_FIRE:     return "\033[48;5;208m";
        case TYPE_WATER:    return C_BG_BLUE;
        case TYPE_GRASS:    return C_BG_GREEN;
        case TYPE_ELECTRIC: return C_BG_YELLOW;
        case TYPE_ICE:      return C_BG_CYAN;
        default:            return C_BG_WHITE;
    }
}

const char *ui_type_label(Type type)
{
    switch (type) {
        case TYPE_NORMAL:   return "NORMAL";
        case TYPE_FIRE:     return "FIRE";
        case TYPE_WATER:    return "WATER";
        case TYPE_GRASS:    return "GRASS";
        case TYPE_ELECTRIC: return "ELECTRIC";
        case TYPE_ICE:      return "ICE";
        default:            return "???";
    }
}

static void print_type_badge(Type type)
{
    printf(" %s" C_BOLD C_WHITE " %s " C_RESET " ",
           ui_type_bg(type), ui_type_label(type));
}

/* ═══════════════════════════════════════════════════════════════════
 *  HP bar builder
 * ═══════════════════════════════════════════════════════════════════ */

static void ui_build_hp_str(char *buf, int bufsize, const Pokemon *p)
{
    int bar_len = 20;
    int filled = 0;
    if (p->max_hp > 0)
        filled = (p->hp * bar_len) / p->max_hp;
    if (filled < 0) filled = 0;
    if (filled > bar_len) filled = bar_len;

    const char *color = C_GREEN;
    float ratio = (p->max_hp > 0) ? (float)p->hp / p->max_hp : 0;
    if (ratio <= 0.25f)
        color = C_RED;
    else if (ratio <= 0.50f)
        color = C_YELLOW;

    char bar[22] = {0};
    for (int i = 0; i < bar_len; i++)
        bar[i] = (i < filled) ? '#' : '-';
    bar[bar_len] = '\0';

    snprintf(buf, bufsize, "%s[%s]%s %d/%d", color, bar, C_WHITE, p->hp, p->max_hp);
}

void ui_hp_bar(const Pokemon *pokemon)
{
    char buf[64];
    ui_build_hp_str(buf, sizeof(buf), pokemon);
    printf("  HP: %s\n", buf);
}

/* ═══════════════════════════════════════════════════════════════════
 *  BOX DRAWING — FireRed-style
 * ═══════════════════════════════════════════════════════════════════ */

static void box_open(void)
{
    printf("╔");
    for (int i = 0; i < IN_W; i++) printf("═");
    printf("╗\n");
}

static void box_close(void)
{
    printf("╚");
    for (int i = 0; i < IN_W; i++) printf("═");
    printf("╝\n");
}

static void box_empty(void)
{
    printf("║");
    for (int i = 0; i < IN_W; i++) putchar(' ');
    printf("║\n");
}

static void box_line(const char *text)
{
    printf("║ %s", text);
    int len = (int)strlen(text) + 1; /* +1 for leading space */
    for (int i = len; i < IN_W; i++) putchar(' ');
    printf("║\n");
}

static void box_line_colored(const char *color, const char *text)
{
    printf("║ %s%s" C_RESET, color, text);
    int len = (int)strlen(text) + 1;
    for (int i = len; i < IN_W; i++) putchar(' ');
    printf("║\n");
}

static void box_line_center(const char *color, const char *text)
{
    int tlen = (int)strlen(text);
    int total = IN_W;
    int left = (total - tlen) / 2;
    int right = total - left - tlen;
    printf("║");
    for (int i = 0; i < left; i++) putchar(' ');
    printf("%s%s" C_RESET, color, text);
    for (int i = 0; i < right; i++) putchar(' ');
    printf("║\n");
}

/* ═══════════════════════════════════════════════════════════════════
 *  TITLE SCREEN — spec 1: yellow "POKESHELL" + welcome box
 * ═══════════════════════════════════════════════════════════════════ */

void ui_print_title(void)
{
    ui_clear();
    printf("\n");
    printf(C_YELLOW C_BOLD);
    printf("    ██████╗  ██████╗ ██╗  ██╗███████╗██╗      ██████╗ ████████╗\n");
    printf("    ██╔══██╗██╔═══██╗██║ ██╔╝██╔════╝██║     ██╔═══██╗╚══██╔══╝\n");
    printf("    ██████╔╝██║   ██║█████╔╝ █████╗  ██║     ██║   ██║   ██║   \n");
    printf("    ██╔═══╝ ██║   ██║██╔═██╗ ██╔══╝  ██║     ██║   ██║   ██║   \n");
    printf("    ██║     ╚██████╔╝██║  ██╗██║     ███████╗╚██████╔╝   ██║   \n");
    printf("    ╚═╝      ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚══════╝ ╚═════╝    ╚═╝   \n");
    printf(C_RESET "\n");
    printf(C_DIM "          ═══════════════════════════════════════\n" C_RESET);
    printf(C_CYAN  "               Your Pokemon adventure awaits!\n" C_RESET);
    printf(C_DIM "          ═══════════════════════════════════════\n" C_RESET);
    printf("\n");
    printf(C_WHITE "  Ready to become a Pokemon Master?\n\n");
    printf("  " C_GREEN "► PRESS ENTER TO START" C_RESET "\n\n");
}

/* ═══════════════════════════════════════════════════════════════════
 *  STARTER SELECTION — spec 2: Oak dialogue + 3-column grid
 * ═══════════════════════════════════════════════════════════════════ */

int ui_starter_select(void)
{
    ui_clear();

    /* Professor Oak dialogue box */
    printf("\n");
    printf(C_BLUE "  ┌──────────────────────────────────────────────┐\n" C_RESET);
    printf(C_BLUE "  │" C_RESET "  " C_BOLD "Prof. Oak:" C_RESET "                              " C_BLUE "│\n" C_RESET);
    printf(C_BLUE "  │" C_RESET "  Welcome to the world of Pokemon!             " C_BLUE "│\n" C_RESET);
    printf(C_BLUE "  │" C_RESET "  I'm Professor Oak.                           " C_BLUE "│\n" C_RESET);
    printf(C_BLUE "  │" C_RESET "  Choose your first Pokemon wisely...           " C_BLUE "│\n" C_RESET);
    printf(C_BLUE "  └──────────────────────────────────────────────┘\n" C_RESET);

    /* Three-column starter grid */
    printf("\n");
    printf(C_WHITE C_BOLD "  ┌─── 1 ──────────┬─── 2 ──────────┬─── 3 ──────────┐\n" C_RESET);

    /* Pokemon names */
    printf("  │");
    printf(" %s%-14s" C_RESET "│", C_RED   C_BOLD, "Charmander");
    printf(" %s%-14s" C_RESET "│", C_BLUE  C_BOLD, "Squirtle");
    printf(" %s%-14s" C_RESET "│", C_GREEN C_BOLD, "Bulbasaur");
    printf("\n");

    /* Types */
    printf("  │");
    printf("  %s%-12s" C_RESET "│", C_RED,   "Fire");
    printf("  %s%-12s" C_RESET "│", C_BLUE,  "Water");
    printf("  %s%-12s" C_RESET "│", C_GREEN, "Grass");
    printf("\n");

    /* Type badges */
    printf("  │");
    printf("  "); print_type_badge(TYPE_FIRE);  printf("          │");
    printf("  "); print_type_badge(TYPE_WATER); printf("          │");
    printf("  "); print_type_badge(TYPE_GRASS); printf("          │");
    printf("\n");

    /* Separator */
    printf("  │");
    printf(" %s──────────────" C_RESET "┼", C_DIM);
    printf(" %s──────────────" C_RESET "┼", C_DIM);
    printf(" %s──────────────" C_RESET "│\n", C_DIM);

    /* Descriptions */
    printf("  │");
    printf(" %s%-14s" C_RESET "│", C_DIM, "Charm sprite");
    printf(" %s%-14s" C_RESET "│", C_DIM, "Tiny turtle");
    printf(" %s%-14s" C_RESET "│", C_DIM, "Seed Pokemon");
    printf("\n");

    printf("  └────────────────┴────────────────┴────────────────┘\n");

    printf("\n");
    printf(C_YELLOW "  Pick your starter: " C_WHITE C_BOLD "[1] [2] [3]" C_YELLOW " ▸ " C_RESET);
    fflush(stdout);

    while (1) {
        char c = ui_get_key();
        if (c == '1') return 0;
        if (c == '2') return 1;
        if (c == '3') return 2;
    }
}

/* ═══════════════════════════════════════════════════════════════════
 *  CHOSEN — brief confirmation
 * ═══════════════════════════════════════════════════════════════════ */

void ui_chosen(const char *name)
{
    ui_clear();
    printf("\n");
    box_open();
    box_line_colored(C_GREEN, "");
    box_line_center(C_GREEN C_BOLD, "You chose ");
    box_line_center(C_GREEN C_BOLD, name);
    box_line_center(C_GREEN, "Your adventure begins now!");
    box_line_colored(C_GREEN, "");
    box_close();
    ui_wait();
}

/* ═══════════════════════════════════════════════════════════════════
 *  BATTLE SCREEN — spec 3: two separate boxes
 * ═══════════════════════════════════════════════════════════════════ */

void ui_print_battle(const Pokemon *player_pokemon, const Pokemon *wild)
{
    ui_clear();
    printf("\n");

    /* ── Wild Pokemon box (top) ── */
    box_open();
    box_line_colored(C_RED C_BOLD, "═══ Wild Pokemon ═══");
    box_empty();
    box_line_colored(ui_type_color(wild->type), wild->name);

    char lvl_buf[64];
    snprintf(lvl_buf, sizeof(lvl_buf), "Lv. %d", wild->level);
    box_line_colored(C_DIM, lvl_buf);

    box_empty();
    print_type_badge(wild->type);
    printf("\n");
    box_empty();
    box_line("HP:");
    /* HP bar inline */
    {
        char hp_buf[64];
        ui_build_hp_str(hp_buf, sizeof(hp_buf), wild);
        printf("║  %s", hp_buf);
        int len = (int)strlen(hp_buf) + 2;
        for (int i = len; i < IN_W; i++) putchar(' ');
        printf("║\n");
    }
    box_close();

    printf("\n");

    /* ── Player Pokemon box (bottom) ── */
    box_open();
    box_line_colored(C_GREEN C_BOLD, "═══ Your Pokemon ═══");
    box_empty();
    box_line_colored(ui_type_color(player_pokemon->type), player_pokemon->name);

    snprintf(lvl_buf, sizeof(lvl_buf), "Lv. %d", player_pokemon->level);
    box_line_colored(C_DIM, lvl_buf);

    box_empty();
    print_type_badge(player_pokemon->type);
    printf("\n");
    box_empty();
    box_line("HP:");
    {
        char hp_buf[64];
        ui_build_hp_str(hp_buf, sizeof(hp_buf), player_pokemon);
        printf("║  %s", hp_buf);
        int len = (int)strlen(hp_buf) + 2;
        for (int i = len; i < IN_W; i++) putchar(' ');
        printf("║\n");
    }
    box_close();

    printf("\n");
}

/* ═══════════════════════════════════════════════════════════════════
 *  BATTLE MENU — spec 4: 2x2 grid with move info
 * ═══════════════════════════════════════════════════════════════════ */

BattleAction ui_battle_menu(const Player *player, const Pokemon *active)
{
    (void)active;
    printf(C_WHITE C_BOLD "  ┌─── 1 ──────┬─── 2 ──────┬─── 3 ──────┬─── 4 ──────┐\n" C_RESET);
    printf("  │");
    printf(" %s%-10s" C_RESET "│", C_RED   C_BOLD, "FIGHT");
    printf(" %s%-10s" C_RESET "│", C_YELLOW, "BAG");
    printf(" %s%-10s" C_RESET "│", C_GREEN, "POKEMON");
    printf(" %s%-10s" C_RESET "│", C_BLUE, "RUN");
    printf("\n");
    printf("  │");
    printf(" %sPokeballs:" C_RESET " ", C_DIM);
    printf("%-10d", player->pokeballs);
    printf("│");
    printf(" %sBadges:" C_RESET "   ", C_DIM);
    printf("%-9d", player->badges);
    printf("│");
    printf("               │\n");
    printf("  └────────────┴────────────┴────────────┴────────────┘\n");
    printf("\n");
    printf(C_YELLOW "  Choose action: " C_WHITE C_BOLD "[1] [2] [3] [4]" C_YELLOW " ▸ " C_RESET);
    fflush(stdout);

    while (1) {
        char c = ui_get_key();
        if (c == '1') return BATTLE_FIGHT;
        if (c == '2') return BATTLE_CATCH;
        if (c == '3') return BATTLE_SWITCH;
        if (c == '4') return BATTLE_RUN;
    }
}

/* ═══════════════════════════════════════════════════════════════════
 *  MOVE SELECT — spec 4: 2-column grid with type, power, PP
 * ═══════════════════════════════════════════════════════════════════ */

int ui_move_select(const Pokemon *pokemon)
{
    printf("\n");
    printf(C_WHITE C_BOLD "  ┌──────────────────────────────┬──────────────────────────────┐\n" C_RESET);
    for (int i = 0; i < pokemon->move_count && i < MAX_MOVES; i += 2) {
        /* Left move */
        const Move *ml = &pokemon->moves[i];
        printf("  │");
        printf(" %s%s%-26s" C_RESET "│", C_BOLD, ui_type_color(ml->type), ml->name);
        /* Right move (if exists) */
        if (i + 1 < pokemon->move_count) {
            const Move *mr = &pokemon->moves[i + 1];
            printf(" %s%s%-26s" C_RESET "│", C_BOLD, ui_type_color(mr->type), mr->name);
        } else {
            printf(" %-27s│", "");
        }
        printf("\n");

        /* Type / Power / PP */
        printf("  │");
        char left_info[32];
        snprintf(left_info, sizeof(left_info), "%s Pwr:%d PP:%d",
                 ui_type_label(ml->type), ml->power, ml->pp);
        printf(" %-27s", left_info);

        if (i + 1 < pokemon->move_count) {
            const Move *mr = &pokemon->moves[i + 1];
            char right_info[32];
            snprintf(right_info, sizeof(right_info), "%s Pwr:%d PP:%d",
                     ui_type_label(mr->type), mr->power, mr->pp);
            printf("│ %-27s", right_info);
        } else {
            printf("│ %-27s", "");
        }
        printf("│\n");

        /* Separator */
        printf("  │");
        printf(" %s──────────────────────────────" C_RESET "┼", C_DIM);
        printf(" %s──────────────────────────────" C_RESET "│\n", C_DIM);
    }
    printf(C_WHITE C_BOLD "  └──────────────────────────────┴──────────────────────────────┘\n" C_RESET);

    /* Pad out to 4 rows if fewer moves */
    printf("\n");
    printf(C_YELLOW "  Choose move: " C_WHITE C_BOLD "[1-4]" C_YELLOW " or " C_WHITE "[0] Back" C_YELLOW " ▸ " C_RESET);
    fflush(stdout);

    while (1) {
        char c = ui_get_key();
        if (c == '0') return -1;
        if (c >= '1' && c <= '4') {
            int idx = c - '1';
            if (idx < pokemon->move_count) return idx;
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════
 *  ENCOUNTER ANIMATION — spec 7: screen flash + "A wild X appeared!"
 * ═══════════════════════════════════════════════════════════════════ */

void ui_encounter_animation(const Pokemon *wild)
{
    /* Flash screen 3 times */
    for (int i = 0; i < 3; i++) {
        ui_clear();
        printf("\n\n\n\n\n");
        printf(C_WHITE C_BOLD "              ┌──────────────────────┐\n");
        printf("              │  ● ● ● ● ● ● ● ●  │\n");
        printf("              │  ● ● ● ● ● ● ● ●  │\n");
        printf("              │  ● ● ● ● ● ● ● ●  │\n");
        printf("              │  ● ● ● ● ● ● ● ●  │\n");
        printf("              │  ● ● ● ● ● ● ● ●  │\n");
        printf("              └──────────────────────┘\n" C_RESET);
        fflush(stdout);
        usleep(150000);
        ui_clear();
        usleep(100000);
    }

    /* Show encounter message */
    ui_clear();
    printf("\n\n\n\n\n\n");
    printf(C_WHITE C_BOLD "              ┌──────────────────────┐\n");
    printf("              │" C_RESET "  " C_RED C_BOLD "A wild %s appeared!" C_RESET C_WHITE "  │\n", wild->name);
    printf(C_WHITE C_BOLD "              └──────────────────────┘\n" C_RESET);
    printf("\n");
    ui_wait();
}

/* ═══════════════════════════════════════════════════════════════════
 *  MOVE RESULT — spec 5: attacker, move, damage, effectiveness
 * ═══════════════════════════════════════════════════════════════════ */

void ui_move_result(const char *attacker, const char *move_name, int damage, float effectiveness)
{
    printf("\n");
    box_open();

    /* Attacker name */
    box_line_center(C_WHITE C_BOLD, attacker);
    box_empty();

    /* Move name */
    char move_line[64];
    snprintf(move_line, sizeof(move_line), "used %s!", move_name);
    box_line_center(C_YELLOW, move_line);
    box_empty();

    /* Damage */
    char dmg_line[64];
    snprintf(dmg_line, sizeof(dmg_line), "Dealt %d damage!", damage);
    box_line_center(C_RED, dmg_line);

    /* Effectiveness */
    if (effectiveness >= 2.0f) {
        box_line_center(C_GREEN C_BOLD, "It's super effective!");
    } else if (effectiveness > 0.0f && effectiveness < 1.0f) {
        box_line_center(C_DIM, "It's not very effective...");
    } else if (effectiveness == 0.0f) {
        box_line_center(C_DIM, "It had no effect...");
    }

    box_close();
    printf("\n");
}

/* ═══════════════════════════════════════════════════════════════════
 *  CATCH ANIMATION — spec 10: pokeball throw with wobble dots
 * ═══════════════════════════════════════════════════════════════════ */

void ui_catch_animation(const Pokemon *pokemon)
{
    (void)pokemon;
    ui_clear();
    printf("\n\n\n\n\n\n");
    printf(C_WHITE C_BOLD "              ┌──────────────────────┐\n");
    printf("              │" C_RESET "  " C_RED "You throw a Pokeball..." C_RESET C_WHITE "  │\n");
    printf(C_WHITE C_BOLD "              └──────────────────────┘\n" C_RESET);
    printf("\n");
    fflush(stdout);
    usleep(400000);

    /* Wobble animation */
    for (int i = 0; i < 3; i++) {
        printf("\r                  ");
        for (int j = 0; j <= i; j++) printf(C_WHITE "● " C_RESET);
        fflush(stdout);
        usleep(400000);
    }
    printf("\n\n");
    fflush(stdout);
}

/* ═══════════════════════════════════════════════════════════════════
 *  CATCH SUCCESS — spec 10: green "Gotcha!"
 * ═══════════════════════════════════════════════════════════════════ */

void ui_catch_success(const Pokemon *pokemon)
{
    ui_clear();
    printf("\n\n\n\n\n\n");
    printf(C_WHITE C_BOLD "              ┌──────────────────────┐\n");
    printf("              │" C_RESET "  " C_GREEN C_BOLD "Gotcha! %s was caught!" C_RESET C_WHITE "  │\n", pokemon->name);
    printf(C_WHITE C_BOLD "              └──────────────────────┘\n" C_RESET);
    printf("\n");
    ui_wait();
}

/* ═══════════════════════════════════════════════════════════════════
 *  CATCH FAILURE — red "broke free"
 * ═══════════════════════════════════════════════════════════════════ */

void ui_catch_failure(const Pokemon *pokemon)
{
    (void)pokemon;
    printf("\n");
    box_open();
    box_line_colored(C_RED C_BOLD, "Oh no!");
    box_line_center(C_RED, "The Pokemon broke free!");
    box_close();
    printf("\n");
}

/* ═══════════════════════════════════════════════════════════════════
 *  RUN RESULT
 * ═══════════════════════════════════════════════════════════════════ */

void ui_run_result(int success)
{
    printf("\n");
    box_open();
    if (success) {
        box_line_center(C_GREEN C_BOLD, "Got away safely!");
    } else {
        box_line_center(C_RED C_BOLD, "Couldn't get away!");
    }
    box_close();
    printf("\n");
}

/* ═══════════════════════════════════════════════════════════════════
 *  FAINTED — spec 12: differentiate wild vs player, show XP
 * ═══════════════════════════════════════════════════════════════════ */

void ui_fainted(const char *name, int xp, int is_wild)
{
    printf("\n");
    box_open();
    if (is_wild && xp > 0) {
        box_line_center(C_GREEN C_BOLD, name);
        box_line_center(C_GREEN, "fainted!");
        box_empty();
        char xp_line[64];
        snprintf(xp_line, sizeof(xp_line), "Gained %d XP!", xp);
        box_line_center(C_YELLOW C_BOLD, xp_line);
    } else {
        box_line_center(C_RED C_BOLD, name);
        box_line_center(C_RED, "fainted!");
    }
    box_close();
    printf("\n");
}

/* ═══════════════════════════════════════════════════════════════════
 *  XP GAIN — quick message
 * ═══════════════════════════════════════════════════════════════════ */

void ui_xp_gain(int xp)
{
    char msg[64];
    snprintf(msg, sizeof(msg), "Gained %d XP!", xp);
    printf("\n");
    box_open();
    box_line_center(C_YELLOW C_BOLD, msg);
    box_close();
    printf("\n");
}

/* ═══════════════════════════════════════════════════════════════════
 *  POKEMON CENTER — spec 9: healing animation
 * ═══════════════════════════════════════════════════════════════════ */

void ui_pokemon_center(void)
{
    ui_clear();
    printf("\n");
    box_open();
    box_line_colored(C_CYAN C_BOLD, "  ♦ Pokemon Center ♦");
    box_empty();
    box_line_colored(C_WHITE, "  Welcome to the Pokemon Center!");
    box_line_colored(C_WHITE, "  We'll heal your Pokemon to full health.");
    box_empty();
    box_line_colored(C_DIM, "  Healing...");
    box_close();
    fflush(stdout);
    usleep(500000);

    /* Healing animation dots */
    for (int i = 0; i < 3; i++) {
        printf("\r  Healing");
        for (int j = 0; j <= i; j++) printf(".");
        for (int j = i + 1; j < 3; j++) printf(" ");
        fflush(stdout);
        usleep(400000);
    }
    printf("\n");

    printf("\n");
    box_open();
    box_line_colored(C_GREEN C_BOLD, "  Your Pokemon are fully healed!");
    box_line_colored(C_GREEN, "  Come back anytime!");
    box_close();
    printf("\n");
    ui_wait();
}

/* ═══════════════════════════════════════════════════════════════════
 *  LEVEL UP — spec 13: show stat increases
 * ═══════════════════════════════════════════════════════════════════ */

void ui_level_up(const Pokemon *pokemon, int old_level, int old_atk, int old_def, int old_hp)
{
    (void)old_level;
    printf("\n");
    box_open();
    box_line_colored(C_YELLOW C_BOLD, "Level Up!");

    char lvl_buf[64];
    snprintf(lvl_buf, sizeof(lvl_buf), "%s grew to Lv. %d!", pokemon->name, pokemon->level);
    box_line_center(C_WHITE C_BOLD, lvl_buf);
    box_empty();

    /* Stat changes */
    char atk_buf[64], def_buf[64], hp_buf[64];
    snprintf(atk_buf, sizeof(atk_buf), "ATK: %d → %d (+%d)", old_atk, pokemon->attack, pokemon->attack - old_atk);
    snprintf(def_buf, sizeof(def_buf), "DEF: %d → %d (+%d)", old_def, pokemon->defense, pokemon->defense - old_def);
    snprintf(hp_buf, sizeof(hp_buf), "HP:  %d → %d (+%d)", old_hp, pokemon->max_hp, pokemon->max_hp - old_hp);

    box_line_center(C_GREEN, atk_buf);
    box_line_center(C_GREEN, def_buf);
    box_line_center(C_GREEN, hp_buf);

    box_close();
    printf("\n");
}

/* ═══════════════════════════════════════════════════════════════════
 *  GYM LEADER — specs for gym encounters
 * ═══════════════════════════════════════════════════════════════════ */

void ui_gym_leader_intro(const char *name)
{
    ui_clear();
    printf("\n");
    box_open();
    box_line_colored(C_MAGENTA C_BOLD, "GYM LEADER");
    box_empty();
    box_line_center(C_WHITE C_BOLD, name);
    box_empty();
    box_line_center(C_MAGENTA, "I accept your challenge!");
    box_close();
    printf("\n");
    ui_wait();
}

void ui_gym_leader_sends(const Pokemon *pokemon)
{
    printf("\n");
    box_open();
    box_line_colored(C_MAGENTA, "Leader sends out");
    box_line_center(C_WHITE C_BOLD, pokemon->name);
    box_close();
    printf("\n");
}

void ui_gym_badge(void)
{
    ui_clear();
    printf("\n\n\n\n");
    box_open();
    box_line_colored(C_MAGENTA C_BOLD, "  ♦ GYM BADGE EARNED ♦");
    box_empty();
    box_line_center(C_WHITE C_BOLD, "Congratulations!");
    box_line_center(C_MAGENTA, "You earned a Gym Badge!");
    box_line_center(C_DIM, "Your Pokemon will be stronger now.");
    box_empty();
    box_line_colored(C_MAGENTA C_BOLD, "  ♦ ♦ ♦ ♦ ♦ ♦ ♦ ♦ ♦ ♦ ♦");
    box_close();
    printf("\n");
    ui_wait();
}

/* ═══════════════════════════════════════════════════════════════════
 *  NO POKEBALLS / TEAM FULL
 * ═══════════════════════════════════════════════════════════════════ */

void ui_no_pokeballs(void)
{
    printf("\n");
    box_open();
    box_line_colored(C_RED C_BOLD, "No Pokeballs left!");
    box_close();
    printf("\n");
}

void ui_team_full(void)
{
    printf("\n");
    box_open();
    box_line_colored(C_RED C_BOLD, "Your team is full!");
    box_line_center(C_DIM, "Release a Pokemon first.");
    box_close();
    printf("\n");
}

/* ═══════════════════════════════════════════════════════════════════
 *  MAP SCREEN — spec 8: polished status bar
 * ═══════════════════════════════════════════════════════════════════ */

void ui_print_map(const Map *map, const Player *player)
{
    ui_clear();
    printf("\n");

    /* ── Viewport (camera follows player) ── */
    int cam_x = player->pos_x - 4;
    int cam_y = player->pos_y - 2;
    if (cam_x < 0) cam_x = 0;
    if (cam_y < 0) cam_y = 0;
    if (cam_x + 9 > map->width)  cam_x = map->width - 9;
    if (cam_y + 5 > map->height) cam_y = map->height - 5;

    for (int y = cam_y; y < cam_y + 5 && y < map->height; y++) {
        for (int x = cam_x; x < cam_x + 9 && x < map->width; x++) {
            if (x == player->pos_x && y == player->pos_y) {
                printf(C_WHITE C_BOLD "@" C_RESET);
            } else {
                TileType t = map->tiles[y][x];
                switch (t) {
                    case TILE_GRASS:      printf(C_GREEN "." C_RESET); break;
                    case TILE_WILD_GRASS: printf(C_GREEN "," C_RESET); break;
                    case TILE_WATER:      printf(C_BLUE "~" C_RESET); break;
                    case TILE_PATH:       printf(C_WHITE "." C_RESET); break;
                    case TILE_TOWN:       printf(C_YELLOW "T" C_RESET); break;
                    case TILE_GYM:        printf(C_MAGENTA "G" C_RESET); break;
                    case TILE_CENTER:     printf(C_CYAN "C" C_RESET); break;
                    case TILE_WALL:       printf(C_DIM "#" C_RESET); break;
                    default:              printf("."); break;
                }
            }
        }
        printf("\n");
    }

    /* ── Status bar ── */
    printf(C_DIM "  ═══════════════════════════════════════════════════\n" C_RESET);
    printf("  %sHP:" C_GREEN " %d/%d" C_RESET "  "
           "%sBall:" C_YELLOW " %d" C_RESET "  "
           "%sBadge:" C_MAGENTA " %d" C_RESET "\n",
           C_DIM, player->team[0].hp, player->team[0].max_hp,
           C_DIM, player->pokeballs,
           C_DIM, player->badges);
    printf(C_DIM "  ═══════════════════════════════════════════════════\n" C_RESET);
    printf(C_YELLOW "  [WASD] Move" C_RESET "  "
           C_WHITE "[T]eam" C_RESET "  "
           C_CYAN "[M]ap" C_RESET "  "
           C_RED "[Q]uit" C_RESET "\n");
}

/* ═══════════════════════════════════════════════════════════════════
 *  TEAM DISPLAY — spec 11: FireRed-style with HP bars for all 6 slots
 * ═══════════════════════════════════════════════════════════════════ */

void ui_show_team(const Player *player)
{
    ui_clear();
    printf("\n");
    box_open();
    box_line_center(C_WHITE C_BOLD, "YOUR TEAM");
    box_close();
    printf("\n");

    for (int i = 0; i < player->team_size; i++) {
        const Pokemon *p = &player->team[i];
        printf(C_WHITE C_BOLD "  [%d] " C_RESET, i + 1);
        printf("%s%-12s" C_RESET, ui_type_color(p->type), p->name);
        printf(C_DIM " Lv.%-3d" C_RESET, p->level);
        printf(" ");
        print_type_badge(p->type);
        printf("  ");
        /* HP bar inline */
        char hp_buf[64];
        ui_build_hp_str(hp_buf, sizeof(hp_buf), p);
        printf("%s", hp_buf);
        printf("\n");
    }

    printf("\n");
    ui_wait();
}

/* ═══════════════════════════════════════════════════════════════════
 *  GAME OVER — spec 14: updated text
 * ═══════════════════════════════════════════════════════════════════ */

void ui_game_over(void)
{
    ui_clear();
    printf("\n\n");
    printf(C_RED C_BOLD);
    printf("    ███████╗██████╗ ██████╗ ███████╗\n");
    printf("    ██╔════╝██╔══██╗██╔═══██╗██╔════╝\n");
    printf("    █████╗  ██║  ██║██║   ██║███████╗\n");
    printf("    ██╔══╝  ██║  ██║██║   ██║╚════██║\n");
    printf("    ██║     ██████╔╝╚██████╔╝███████║\n");
    printf("    ╚═╝     ╚═════╝  ╚═════╝ ╚══════╝\n");
    printf(C_RESET "\n");
    box_open();
    box_line_colored(C_RED C_BOLD, "  GAME OVER");
    box_empty();
    box_line_center(C_WHITE, "Your journey has ended...");
    box_line_center(C_DIM, "All your Pokemon have fainted.");
    box_empty();
    box_line_center(C_DIM, "But a true trainer never gives up!");
    box_close();
    printf("\n");
    ui_wait();
}

/* ═══════════════════════════════════════════════════════════════════
 *  VICTORY — spec 14: updated text
 * ═══════════════════════════════════════════════════════════════════ */

void ui_victory(void)
{
    ui_clear();
    printf("\n\n");
    printf(C_YELLOW C_BOLD);
    printf("    ██╗   ██╗██╗███████╗███████╗ ██████╗\n");
    printf("    ██║   ██║██║██╔════╝██╔════╝██╔════╝\n");
    printf("    ██║   ██║██║█████╗  ███████╗██║     \n");
    printf("    ╚██╗ ██╔╝██║██╔══╝  ╚════██║██║     \n");
    printf("     ╚████╔╝ ██║███████╗███████║╚██████╗\n");
    printf("      ╚═══╝  ╚═╝╚══════╝╚══════╝ ╚═════╝\n");
    printf(C_RESET "\n");
    box_open();
    box_line_colored(C_YELLOW C_BOLD, "  ♦ CHAMPION ♦");
    box_empty();
    box_line_center(C_WHITE C_BOLD, "Congratulations!");
    box_line_center(C_YELLOW, "You defeated all the Gyms!");
    box_line_center(C_WHITE, "You are the Pokemon Champion!");
    box_empty();
    box_line_colored(C_YELLOW C_BOLD, "  ♦ ♦ ♦ ♦ ♦ ♦ ♦ ♦ ♦ ♦ ♦");
    box_close();
    printf("\n");
    ui_wait();
}

/* ═══════════════════════════════════════════════════════════════════
 *  MESSAGE — generic box (preserved from original)
 * ═══════════════════════════════════════════════════════════════════ */

void ui_message(const char *msg)
{
    box_open();
    box_line(msg);
    box_close();
}
