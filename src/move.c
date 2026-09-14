#include "move.h"
#include <string.h>

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
    // TODO: read from data/moves.txt
    // for now, return 0
    (void)buffer;
    (void)max_moves;
    return 0;
}
