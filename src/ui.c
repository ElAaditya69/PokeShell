#include "ui.h"
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

int ui_starter_select(Pokemon starters[], int count)
{
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
    printf("\n  === WILD %s APPEARED! ===\n\n", wild->name);
    ui_print_hp_bar(wild);
    printf("\n");
    ui_print_hp_bar(player_pokemon);
    printf("\n");
}

BattleAction ui_battle_menu(const Player *player)
{
    (void)player;
    printf("  What will you do?\n");
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

void ui_message(const char *msg)
{
    printf("  %s\n", msg);
}

char ui_get_key(void)
{
    char c;
    // TODO: implement raw terminal input (no enter needed)
    scanf(" %c", &c);
    while (getchar() != '\n');
    return c;
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
