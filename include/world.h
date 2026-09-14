#ifndef POKESHELL_WORLD_H
#define POKESHELL_WORLD_H

// map tiles
typedef enum {
    TILE_PATH,
    TILE_GRASS,
    TILE_WATER,
    TILE_WALL,
    TILE_TOWN,
    TILE_CENTER,
    TILE_GYM,
    TILE_WILD_GRASS
} TileType;

#define MAX_MAP_SIZE 32

// a map
typedef struct {
    TileType tiles[MAX_MAP_SIZE][MAX_MAP_SIZE];
    int width;
    int height;
    int encounter_rate;   // 1 in N steps triggers encounter in grass
    int gym_defeated;     // 1 if the gym leader has been beaten
    char name[32];
} Map;

// create a default starting map
Map world_create_starting_map(void);

// try to move the player in a direction (dx, dy)
// returns 1 if moved successfully, 0 if blocked
int world_move_player(Map *map, int *px, int *py, int dx, int dy);

// check if the current tile triggers a wild encounter
int world_check_encounter(const Map *map, int x, int y);

// get the tile at a position
TileType world_get_tile(const Map *map, int x, int y);

#endif
