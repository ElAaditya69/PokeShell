#include "types.h"
#include <string.h>
#include <ctype.h>

// effectiveness chart
// rows = attacker, columns = defender
// 1.0 = normal, 2.0 = super effective, 0.5 = not very effective, 0.0 = immune
static const float effectiveness[TYPE_COUNT][TYPE_COUNT] = {
    /*              NORMAL  FIRE  WATER GRASS ELECTRIC ICE */
    /* NORMAL  */ {  1.0,  1.0,  1.0,  1.0,  1.0,    1.0 },
    /* FIRE    */ {  1.0,  0.5,  0.5,  2.0,  1.0,    2.0 },
    /* WATER   */ {  1.0,  2.0,  0.5,  0.5,  1.0,    1.0 },
    /* GRASS   */ {  1.0,  0.5,  2.0,  0.5,  1.0,    1.0 },
    /* ELECTRIC*/ {  1.0,  1.0,  2.0,  0.5,  0.5,    1.0 },
    /* ICE     */ {  1.0,  0.5,  0.5,  2.0,  1.0,    0.5 },
};

float type_effectiveness(Type attacker, Type defender)
{
    if (attacker < 0 || attacker >= TYPE_COUNT) return 1.0;
    if (defender < 0 || defender >= TYPE_COUNT) return 1.0;
    return effectiveness[attacker][defender];
}

const char *type_name(Type type)
{
    static const char *names[] = {
        "Normal", "Fire", "Water",
        "Grass", "Electric", "Ice"
    };
    if (type < 0 || type >= TYPE_COUNT) return "???";
    return names[type];
}

Type type_from_name(const char *name)
{
    /* compare case-insensitively so UPPERCASE from moves.txt also works */
    char buf[32];
    int i;
    for (i = 0; name[i] && i < (int)sizeof(buf) - 1; i++)
        buf[i] = (char)toupper((unsigned char)name[i]);
    buf[i] = '\0';

    if (strcmp(buf, "NORMAL") == 0)   return TYPE_NORMAL;
    if (strcmp(buf, "FIRE") == 0)     return TYPE_FIRE;
    if (strcmp(buf, "WATER") == 0)    return TYPE_WATER;
    if (strcmp(buf, "GRASS") == 0)    return TYPE_GRASS;
    if (strcmp(buf, "ELECTRIC") == 0) return TYPE_ELECTRIC;
    if (strcmp(buf, "ICE") == 0)      return TYPE_ICE;
    return TYPE_NORMAL;  // default
}
