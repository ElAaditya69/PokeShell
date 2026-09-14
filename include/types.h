#ifndef POKESHELL_TYPES_H
#define POKESHELL_TYPES_H

// all the pokemon types
typedef enum {
    TYPE_NORMAL,
    TYPE_FIRE,
    TYPE_WATER,
    TYPE_GRASS,
    TYPE_ELECTRIC,
    TYPE_ICE,
    TYPE_COUNT
} Type;

// return the effectiveness multiplier when attacker hits defender
// e.g. fire attacking grass = 2.0, water attacking fire = 2.0
float type_effectiveness(Type attacker, Type defender);

// get the name of a type as a string
const char *type_name(Type type);

// convert a string to a Type enum
Type type_from_name(const char *name);

#endif
