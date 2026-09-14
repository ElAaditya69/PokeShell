#include "move.h"
#include <string.h>
#include <stdio.h>

Move move_create(const char *name, Type type, int power, int accuracy, int pp)
{
    Move m;
    memset(&m, 0, sizeof(Move));

    strncpy(m.name, name, sizeof(m.name) - 1);
    m.type = type;
    m.power = power;
    m.accuracy = accuracy;
    m.pp = pp;
    m.max_pp = pp;

    return m;
}

int move_load_all(Move *buffer, int max_moves)
{
    FILE *f = fopen("data/moves.txt", "r");
    if (!f) return 0;

    int count = 0;
    char line[256];

    while (count < max_moves && fgets(line, sizeof(line), f)) {
        // skip empty lines and comments
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\0')
            continue;

        char name[32];
        char type_str[16];
        int power, accuracy, pp;

        if (sscanf(line, "%31s %15s %d %d %d", name, type_str, &power, &accuracy, &pp) == 5) {
            buffer[count] = move_create(name, type_from_name(type_str), power, accuracy, pp);
            count++;
        }
    }

    fclose(f);
    return count;
}

void move_get_starter_moves(const char *starter_name, Move moves[], int *count)
{
    *count = 0;

    if (strcmp(starter_name, "Bulbasaur") == 0) {
        moves[0] = move_create("Tackle",    TYPE_NORMAL, 40, 100, 35);
        moves[1] = move_create("VineWhip",  TYPE_GRASS,  45, 100, 25);
        *count = 2;
    } else if (strcmp(starter_name, "Charmander") == 0) {
        moves[0] = move_create("Scratch", TYPE_NORMAL, 40, 100, 35);
        moves[1] = move_create("Ember",   TYPE_FIRE,   40, 100, 25);
        *count = 2;
    } else if (strcmp(starter_name, "Squirtle") == 0) {
        moves[0] = move_create("Tackle",   TYPE_NORMAL, 40, 100, 35);
        moves[1] = move_create("WaterGun", TYPE_WATER,  40, 100, 25);
        *count = 2;
    }
}
