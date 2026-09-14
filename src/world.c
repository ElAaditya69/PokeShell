#include "world.h"
#include <stdlib.h>
#include <string.h>

/* helper: fill a rectangular region with a tile */
static void fill_rect(Map *map, int x1, int y1, int x2, int y2, TileType t)
{
    for (int y = y1; y <= y2; y++)
        for (int x = x1; x <= x2; x++)
            map->tiles[y][x] = t;
}

Map world_create_starting_map(void)
{
    Map map;
    memset(&map, 0, sizeof(Map));

    map.width = 30;
    map.height = 25;
    map.encounter_rate = 5;
    map.gym_defeated = 0;
    strncpy(map.name, "Kanto Region", sizeof(map.name) - 1);

    /* start everything as walls */
    fill_rect(&map, 0, 0, 29, 24, TILE_WALL);

    /* ============================================================
     * Town (top-left, rows 1-6, cols 1-7)
     * player starts at (2,2) inside the town
     * ============================================================ */
    fill_rect(&map, 1, 1, 7, 6, TILE_TOWN);

    /* pokemon center inside town */
    map.tiles[3][4] = TILE_CENTER;

    /* path from town heading east (row 3) */
    for (int x = 8; x <= 28; x++)
        map.tiles[3][x] = TILE_PATH;

    /* path from town heading south (col 4) */
    for (int y = 7; y <= 11; y++)
        map.tiles[y][4] = TILE_PATH;

    /* ============================================================
     * Route 1 (rows 6-12, cols 5-20)
     * easy encounters — mostly grass
     * ============================================================ */
    fill_rect(&map, 5, 6, 20, 12, TILE_GRASS);

    /* main path through Route 1 (row 3 east, then south) */
    for (int x = 5; x <= 20; x++)
        map.tiles[3][x] = TILE_PATH;

    /* a winding path through Route 1 */
    for (int x = 8; x <= 14; x++)
        map.tiles[9][x] = TILE_PATH;

    /* some water for decoration */
    map.tiles[7][18] = TILE_WATER;
    map.tiles[7][19] = TILE_WATER;
    map.tiles[8][18] = TILE_WATER;
    map.tiles[8][19] = TILE_WATER;

    /* ============================================================
     * Narrow path / bridge (row 12-13, cols 14-17)
     * ============================================================ */
    fill_rect(&map, 14, 11, 17, 13, TILE_PATH);

    /* water around the bridge */
    fill_rect(&map, 11, 12, 13, 13, TILE_WATER);
    fill_rect(&map, 18, 12, 19, 13, TILE_WATER);

    /* ============================================================
     * Route 2 (rows 13-21, cols 8-27)
     * harder encounters — wild grass
     * ============================================================ */
    fill_rect(&map, 8, 13, 27, 21, TILE_WILD_GRASS);

    /* path through Route 2 */
    for (int x = 14; x <= 24; x++)
        map.tiles[15][x] = TILE_PATH;

    /* some regular grass patches in Route 2 */
    fill_rect(&map, 9, 14, 12, 16, TILE_GRASS);

    /* water features in Route 2 */
    fill_rect(&map, 22, 17, 25, 19, TILE_WATER);

    /* path to gym */
    for (int y = 16; y <= 20; y++)
        map.tiles[y][24] = TILE_PATH;

    /* ============================================================
     * Gym (rows 19-21, cols 25-28)
     * ============================================================ */
    fill_rect(&map, 25, 19, 28, 21, TILE_TOWN);
    map.tiles[20][26] = TILE_GYM;

    return map;
}

int world_move_player(Map *map, int *px, int *py, int dx, int dy)
{
    int nx = *px + dx;
    int ny = *py + dy;

    /* bounds check */
    if (nx < 0 || nx >= map->width || ny < 0 || ny >= map->height) return 0;

    TileType tile = map->tiles[ny][nx];

    /* can't walk into walls or water */
    if (tile == TILE_WALL || tile == TILE_WATER) return 0;

    /* gym is blocked until defeated */
    if (tile == TILE_GYM && !map->gym_defeated) return 0;

    *px = nx;
    *py = ny;
    return 1;
}

int world_check_encounter(const Map *map, int x, int y)
{
    TileType t = map->tiles[y][x];
    if (t != TILE_GRASS && t != TILE_WILD_GRASS) return 0;
    return (rand() % map->encounter_rate == 0);
}

TileType world_get_tile(const Map *map, int x, int y)
{
    if (x < 0 || x >= map->width || y < 0 || y >= map->height) return TILE_WALL;
    return map->tiles[y][x];
}
