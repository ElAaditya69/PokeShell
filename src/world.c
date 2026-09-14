#include "world.h"
#include <stdlib.h>
#include <string.h>

Map world_create_starting_map(void)
{
    Map map;
    memset(&map, 0, sizeof(Map));

    map.width = 20;
    map.height = 20;
    map.encounter_rate = 5;  // 1 in 5 steps in grass
    strncpy(map.name, "Route 1", sizeof(map.name) - 1);

    // fill with paths
    for (int y = 0; y < map.height; y++) {
        for (int x = 0; x < map.width; x++) {
            // walls on edges
            if (x == 0 || y == 0 || x == map.width - 1 || y == map.height - 1) {
                map.tiles[y][x] = TILE_WALL;
            } else {
                map.tiles[y][x] = TILE_PATH;
            }
        }
    }

    // add some grass patches
    for (int y = 8; y < 14; y++) {
        for (int x = 5; x < 15; x++) {
            map.tiles[y][x] = TILE_GRASS;
        }
    }

    // town in the top left
    for (int y = 1; y < 5; y++) {
        for (int x = 1; x < 5; x++) {
            map.tiles[y][x] = TILE_TOWN;
        }
    }

    return map;
}

int world_move_player(Map *map, int *px, int *py, int dx, int dy)
{
    int nx = *px + dx;
    int ny = *py + dy;

    // bounds check
    if (nx < 0 || nx >= map->width || ny < 0 || ny >= map->height) return 0;

    // can't walk into walls or water
    TileType tile = map->tiles[ny][nx];
    if (tile == TILE_WALL || tile == TILE_WATER) return 0;

    *px = nx;
    *py = ny;
    return 1;
}

int world_check_encounter(const Map *map, int x, int y)
{
    if (map->tiles[y][x] != TILE_GRASS) return 0;
    return (rand() % map->encounter_rate == 0);
}

TileType world_get_tile(const Map *map, int x, int y)
{
    if (x < 0 || x >= map->width || y < 0 || y >= map->height) return TILE_WALL;
    return map->tiles[y][x];
}
