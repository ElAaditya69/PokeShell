#include "types.h"
#include <string.h>

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
    if (strcmp(name, "Normal") == 0) return TYPE_NORMAL;
    if (strcmp(name, "Fire") == 0)   return TYPE_FIRE;
    if (strcmp(name, "Water") == 0)  return TYPE_WATER;
    if (strcmp(name, "Grass") == 0)  return TYPE_GRASS;
    if (strcmp(name, "Electric") == 0) return TYPE_ELECTRIC;
    if (strcmp(name, "Ice") == 0)    return TYPE_ICE;
    return TYPE_NORMAL;  // default
}
