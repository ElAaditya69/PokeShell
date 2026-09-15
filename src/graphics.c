#include "game.h"
#include "pokemon.h"
#include "player.h"
#include "battle.h"
#include "world.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TILE_SIZE 32
#define VIEW_W 800
#define VIEW_H 600
#define UI_H 100
#define MAP_VIEW_H (VIEW_H - UI_H)

static const Color C_GRASS      = {76,  153, 0,   255};
static const Color C_GRASS_DARK = {60,  130, 0,   255};
static const Color C_WATER      = {0,   102, 204, 255};
static const Color C_PATH       = {210, 180, 140, 255};
static const Color C_WALL       = {64,  64,  64,  255};
static const Color C_TOWN       = {255, 230, 128, 255};
static const Color C_CENTER     = {220, 50,  50,  255};
static const Color C_GYM        = {200, 50,  200, 255};
static const Color C_WILD       = {50,  130, 0,   255};
static const Color C_PANEL      = {30,  30,  50,  255};
static const Color C_PANEL_HI   = {50,  50,  80,  255};
static const Color C_HP_GREEN   = {70,  200, 70,  255};
static const Color C_HP_RED     = {220, 60,  60,  255};
static const Color C_YELLOW     = {255, 215, 0,   255};
static const Color C_BLUE       = {100, 149, 237, 255};
static const Color C_RED        = {220, 60,  60,  255};
static const Color C_WHITE      = {255, 255, 255, 255};
static const Color C_BLACK      = {0,   0,   0,   255};
static const Color C_LIGHTGRAY  = {200, 200, 200, 255};
static const Color C_DARKGRAY   = {80,  80,  80,  255};

/* ---- helper drawing functions ---- */

static void draw_text_shadow(int x, int y, const char *t, int sz, Color c)
{
    DrawText(t, x + 1, y + 1, sz, C_BLACK);
    DrawText(t, x, y, sz, c);
}

static void draw_hp_bar(int x, int y, int w, int cur, int mx)
{
    DrawRectangle(x, y, w, 12, C_BLACK);
    if (mx > 0) {
        int fill = (int)((float)cur / mx * (w - 2));
        Color c = (cur > mx / 2) ? C_HP_GREEN : C_HP_RED;
        DrawRectangle(x + 1, y + 1, fill, 10, c);
    }
}

static void draw_rounded(int x, int y, int w, int h, Color c)
{
    DrawRectangleRounded((Rectangle){(float)x, (float)y, (float)w, (float)h}, 0.3f, 6, c);
}

static void draw_rounded_outline(int x, int y, int w, int h, Color fill, Color border)
{
    DrawRectangleRounded((Rectangle){(float)x, (float)y, (float)w, (float)h}, 0.3f, 6, fill);
    DrawRectangleRoundedLines((Rectangle){(float)x, (float)y, (float)w, (float)h}, 0.3f, 6, border);
}

/* ---- title screen ---- */

static void draw_title(void)
{
    ClearBackground(C_BLACK);
    for (int r = 0; r < VIEW_H / TILE_SIZE + 1; r++)
        for (int c = 0; c < VIEW_W / TILE_SIZE + 1; c++)
            DrawRectangle(c * TILE_SIZE, r * TILE_SIZE, TILE_SIZE, TILE_SIZE,
                          ((r + c) % 2 == 0) ? C_GRASS : C_GRASS_DARK);

    DrawRectangle(150, 90, 500, 310, (Color){0, 0, 0, 200});

    draw_text_shadow(280, 115, "POKESHELL", 60, C_YELLOW);
    draw_text_shadow(230, 195, "A Terminal Pokemon Adventure", 20, C_WHITE);
    draw_text_shadow(265, 290, "[ ENTER ] Start Game", 22, C_WHITE);
    draw_text_shadow(285, 330, "[ Q ] Quit", 20, C_LIGHTGRAY);
}

/* ---- starter selection ---- */

typedef struct {
    const char *name;
    Type type;
    int hp, atk, def;
    const char *label;
} StarterInfo;

static int draw_starter_select(void)
{
    static const StarterInfo starters[] = {
        {"Bulbasaur", TYPE_GRASS,  45, 49, 49, "GRASS"},
        {"Charmander", TYPE_FIRE,  39, 52, 43, "FIRE"},
        {"Squirtle",   TYPE_WATER, 44, 48, 65, "WATER"},
    };
    static int sel = 0;

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))   sel = (sel + 2) % 3;
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) sel = (sel + 1) % 3;

    ClearBackground(C_BLACK);

    draw_text_shadow(260, 30, "CHOOSE YOUR PARTNER", 28, C_YELLOW);

    for (int i = 0; i < 3; i++) {
        int bx = 80 + i * 220, by = 100;
        bool hi = (i == sel);
        Color bg = hi ? C_PANEL_HI : C_PANEL;
        draw_rounded_outline(bx, by, 190, 420, bg, hi ? C_YELLOW : C_DARKGRAY);

        Color tc = C_WHITE;
        if (starters[i].type == TYPE_FIRE)  tc = C_RED;
        if (starters[i].type == TYPE_WATER) tc = C_BLUE;
        if (starters[i].type == TYPE_GRASS) tc = C_GRASS;

        DrawRectangle(bx + 40, by + 30, 110, 110, (Color){tc.r/3, tc.g/3, tc.b/3, 255});
        draw_text_shadow(bx + 55, by + 65, starters[i].name, 18, tc);

        int sy = by + 170;
        draw_text_shadow(bx + 20, sy,      "Type:", 16, C_LIGHTGRAY);
        draw_text_shadow(bx + 20, sy + 25, starters[i].label, 18, tc);
        draw_text_shadow(bx + 20, sy + 70, "HP:",  14, C_LIGHTGRAY);
        DrawRectangle(bx + 60, sy + 70, 80, 14, C_DARKGRAY);
        DrawRectangle(bx + 60, sy + 70,
                      (int)(80.0f * starters[i].hp / 65), 14, C_HP_GREEN);
        draw_text_shadow(bx + 20, sy + 100, "ATK:", 14, C_LIGHTGRAY);
        DrawRectangle(bx + 60, sy + 100, 80, 14, C_DARKGRAY);
        DrawRectangle(bx + 60, sy + 100,
                      (int)(80.0f * starters[i].atk / 65), 14, C_HP_RED);
        draw_text_shadow(bx + 20, sy + 130, "DEF:", 14, C_LIGHTGRAY);
        DrawRectangle(bx + 60, sy + 130, 80, 14, C_DARKGRAY);
        DrawRectangle(bx + 60, sy + 130,
                      (int)(80.0f * starters[i].def / 65), 14, C_BLUE);

        if (hi) draw_text_shadow(bx + 55, by + 380, "YOU", 18, C_YELLOW);
    }

    draw_text_shadow(230, 545, "[W/S] Select   [ENTER] Confirm", 16, C_LIGHTGRAY);

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        return sel;
    return -1;
}

/* ---- explore mode ---- */

static void draw_explore(const Game *g)
{
    ClearBackground(C_BLACK);

    /* Camera2D: center viewport on the player */
    Camera2D camera = {0};
    camera.target   = (Vector2){
        (float)g->player.pos_x * TILE_SIZE + TILE_SIZE / 2.0f,
        (float)g->player.pos_y * TILE_SIZE + TILE_SIZE / 2.0f
    };
    camera.offset   = (Vector2){ VIEW_W / 2.0f, MAP_VIEW_H / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom     = 1.0f;

    BeginMode2D(camera);

    /* draw tiles */
    for (int r = 0; r < g->current_map.height; r++) {
        for (int c = 0; c < g->current_map.width; c++) {
            int sx = c * TILE_SIZE;
            int sy = r * TILE_SIZE;

            Color col;
            switch (g->current_map.tiles[r][c]) {
                case TILE_GRASS:      col = C_GRASS;  break;
                case TILE_WATER:      col = C_WATER;  break;
                case TILE_PATH:       col = C_PATH;   break;
                case TILE_WALL:       col = C_WALL;   break;
                case TILE_TOWN:       col = C_TOWN;   break;
                case TILE_CENTER:     col = C_CENTER;  break;
                case TILE_GYM:        col = C_GYM;    break;
                case TILE_WILD_GRASS: col = C_WILD;   break;
            }
            DrawRectangle(sx, sy, TILE_SIZE, TILE_SIZE, col);

            if (g->current_map.tiles[r][c] == TILE_WALL) {
                DrawRectangle(sx + 10, sy + 14, 12, 18, (Color){75, 55, 35, 255});
                DrawCircle(sx + 16, sy + 10, 12, (Color){30, 120, 30, 255});
            }
            if (g->current_map.tiles[r][c] == TILE_GRASS &&
                ((r + c) % 5 == 0))
                DrawRectangle(sx + 8, sy + 8, 3, 3, C_GRASS_DARK);
            if (g->current_map.tiles[r][c] == TILE_WILD_GRASS) {
                DrawRectangle(sx + 4,  sy + 6,  3, 5, C_GRASS);
                DrawRectangle(sx + 14, sy + 4,  3, 7, C_GRASS);
                DrawRectangle(sx + 24, sy + 8,  3, 4, C_GRASS);
            }
        }
    }

    /* player: bright white square with blue border */
    {
        int px = g->player.pos_x * TILE_SIZE;
        int py = g->player.pos_y * TILE_SIZE;
        DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, C_BLUE);
        DrawRectangle(px + 2, py + 2, TILE_SIZE - 4, TILE_SIZE - 4, C_WHITE);
    }

    EndMode2D();

    /* minimap (screen-space overlay) */
    {
        int mw = 112, mh = 112;
        int mx = VIEW_W - mw - 10, my = 10;
        DrawRectangle(mx - 2, my - 2, mw + 4, mh + 4, C_BLACK);
        int tw = mw / g->current_map.width;
        int th = mh / g->current_map.height;
        if (tw < 1) tw = 1; if (th < 1) th = 1;
        for (int r = 0; r < g->current_map.height; r++) {
            for (int c = 0; c < g->current_map.width; c++) {
                Color mc;
                switch (g->current_map.tiles[r][c]) {
                    case TILE_GRASS: case TILE_WILD_GRASS: mc = C_GRASS; break;
                    case TILE_WATER:     mc = C_WATER;  break;
                    case TILE_PATH: case TILE_TOWN: mc = C_TOWN; break;
                    case TILE_WALL:      mc = C_WALL;   break;
                    case TILE_CENTER:    mc = C_CENTER;  break;
                    case TILE_GYM:       mc = C_GYM;    break;
                }
                DrawRectangle(mx + c * tw, my + r * th, tw, th, mc);
            }
        }
        DrawRectangle(mx + g->player.pos_x * tw, my + g->player.pos_y * th,
                      tw > 2 ? 2 : 1, th > 2 ? 2 : 1, C_WHITE);
    }

    /* UI panel (screen-space) */
    DrawRectangle(0, MAP_VIEW_H, VIEW_W, UI_H, C_PANEL);
    DrawRectangle(0, MAP_VIEW_H, VIEW_W, 2, C_YELLOW);

    if (g->player.team_size > 0) {
        const Pokemon *pk = &g->player.team[0];
        draw_text_shadow(15, MAP_VIEW_H + 12, pk->name, 20, C_WHITE);
        draw_text_shadow(15, MAP_VIEW_H + 38, "Lv", 14, C_LIGHTGRAY);
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", pk->level);
        draw_text_shadow(35, MAP_VIEW_H + 38, buf, 14, C_YELLOW);
        draw_hp_bar(15, MAP_VIEW_H + 60, 160, pk->hp, pk->max_hp);
        snprintf(buf, sizeof(buf), "%d/%d", pk->hp, pk->max_hp);
        draw_text_shadow(180, MAP_VIEW_H + 60, buf, 12, C_WHITE);
    }

    draw_text_shadow(420, MAP_VIEW_H + 12, "Pokeballs:", 14, C_LIGHTGRAY);
    { char b[8]; snprintf(b, sizeof(b), "%d", g->player.pokeballs);
      draw_text_shadow(540, MAP_VIEW_H + 12, b, 14, C_YELLOW); }

    draw_text_shadow(420, MAP_VIEW_H + 34, "Badges:", 14, C_LIGHTGRAY);
    { char b[8]; snprintf(b, sizeof(b), "%d", g->player.badges);
      draw_text_shadow(510, MAP_VIEW_H + 34, b, 14, C_YELLOW); }

    draw_text_shadow(420, MAP_VIEW_H + 60, "[WASD] Move", 12, C_LIGHTGRAY);
    draw_text_shadow(560, MAP_VIEW_H + 60, "[T] Team",    12, C_LIGHTGRAY);

    draw_text_shadow(15, MAP_VIEW_H + 80,
        "[Q] Quit", 12, C_LIGHTGRAY);
}

static void update_explore(Game *g)
{
    if (player_all_fainted(&g->player)) { g->state = STATE_GAME_OVER; return; }

    if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE)) { g->state = STATE_GAME_OVER; return; }

    int dx = 0, dy = 0;
    if (IsKeyPressed(KEY_W)) dy = -1;
    if (IsKeyPressed(KEY_S)) dy =  1;
    if (IsKeyPressed(KEY_A)) dx = -1;
    if (IsKeyPressed(KEY_D)) dx =  1;

    if (dx != 0 || dy != 0) {
        world_move_player(&g->current_map, &g->player.pos_x, &g->player.pos_y, dx, dy);
    }

    TileType tile = world_get_tile(&g->current_map, g->player.pos_x, g->player.pos_y);
    if (tile == TILE_CENTER) {
        player_heal_team(&g->player);
        return;
    }
    if (tile == TILE_GYM && !g->current_map.gym_defeated) {
        g->is_gym_battle = 1;
        g->gym_team_size = GYM_TEAM_SIZE;
        g->gym_current = 0;
        g->gym_team[0] = pokemon_create_starter("Squirtle",   TYPE_WATER, 44, 48, 65);
        g->gym_team[1] = pokemon_create_starter("Bulbasaur",  TYPE_GRASS, 45, 49, 49);
        g->gym_team[2] = pokemon_create_starter("Charmander", TYPE_FIRE,  39, 52, 43);
        for (int i = 0; i < GYM_TEAM_SIZE; i++)
            while (g->gym_team[i].level < 12 + i)
                pokemon_gain_xp(&g->gym_team[i], g->gym_team[i].xp_to_next);
        g->wild_pokemon = g->gym_team[0];
        g->state = STATE_BATTLE;
        return;
    }
    if (world_check_encounter(&g->current_map, g->player.pos_x, g->player.pos_y)) {
        int lvl;
        if (g->player.pos_y <= 6)       lvl = 3;
        else if (g->player.pos_y <= 12)  lvl = 3 + (g->player.pos_y - 7) / 2;
        else                              lvl = 8 + (g->player.pos_y - 13) / 3;
        g->wild_pokemon = battle_generate_wild(lvl);
        g->state = STATE_BATTLE;
    }
}

/* ---- battle mode ---- */

static Color type_col(Type t)
{
    switch (t) {
        case TYPE_NORMAL:   return C_WHITE;
        case TYPE_FIRE:     return C_RED;
        case TYPE_WATER:    return C_BLUE;
        case TYPE_GRASS:    return C_GRASS;
        case TYPE_ELECTRIC: return C_YELLOW;
        case TYPE_ICE:      return (Color){150, 220, 255, 255};
        default:            return C_WHITE;
    }
    return C_WHITE;
}

static void draw_pokemon_sprite(int x, int y, Type t, int size)
{
    Color c = type_col(t);
    Color dk = (Color){c.r/3, c.g/3, c.b/3, 255};
    DrawRectangle(x, y, size, size, dk);
    DrawCircle(x + size/2, y + size/2, size/3, c);
    draw_text_shadow(x + size/4 - 2, y + size/2 - 6, ">>", 14, C_WHITE);
}

static void draw_battle(Game *g)
{
    /* green gradient background */
    for (int y = 0; y < VIEW_H; y++) {
        float t = (float)y / VIEW_H;
        unsigned char r = (unsigned char)(30 + 40 * t);
        unsigned char gr = (unsigned char)(120 + 60 * t);
        unsigned char b = (unsigned char)(20 + 20 * t);
        DrawRectangle(0, y, VIEW_W, 1, (Color){r, gr, b, 255});
    }

    /* ground strip */
    DrawRectangle(0, 350, VIEW_W, 50, (Color){50, 110, 30, 255});

    /* enemy pokemon (top-right) */
    draw_pokemon_sprite(580, 80, g->wild_pokemon.type, 80);

    /* enemy name plate (top-left area) */
    draw_rounded_outline(380, 40, 220, 60, (Color){20, 20, 40, 220}, C_DARKGRAY);
    draw_text_shadow(395, 48, g->wild_pokemon.name, 18, C_WHITE);
    { char b[32]; snprintf(b, sizeof(b), "Lv %d", g->wild_pokemon.level);
      draw_text_shadow(395, 72, b, 14, C_YELLOW); }

    /* enemy HP bar */
    draw_hp_bar(380, 100, 200, g->wild_pokemon.hp, g->wild_pokemon.max_hp);
    { char b[16]; snprintf(b, sizeof(b), "%d/%d", g->wild_pokemon.hp, g->wild_pokemon.max_hp);
      draw_text_shadow(585, 100, b, 12, C_WHITE); }
    draw_text_shadow(365, 100, "HP", 12, C_LIGHTGRAY);

    /* player pokemon (bottom-left) */
    draw_pokemon_sprite(60, 240, g->player.team[0].type, 80);

    /* player name plate */
    draw_rounded_outline(200, 280, 220, 60, (Color){20, 20, 40, 220}, C_DARKGRAY);
    draw_text_shadow(215, 288, g->player.team[0].name, 18, C_WHITE);
    { char b[32]; snprintf(b, sizeof(b), "Lv %d", g->player.team[0].level);
      draw_text_shadow(215, 312, b, 14, C_YELLOW); }

    /* player HP bar */
    draw_hp_bar(200, 330, 200, g->player.team[0].hp, g->player.team[0].max_hp);
    { char b[16]; snprintf(b, sizeof(b), "%d/%d", g->player.team[0].hp, g->player.team[0].max_hp);
      draw_text_shadow(405, 330, b, 12, C_WHITE); }
    draw_text_shadow(185, 330, "HP", 12, C_LIGHTGRAY);
}

/* battle move result state */
static int  s_move_phase = 0;   /* 0 = menu, 1 = result, 2 = enemy, 3 = done */
static int  s_sel = 0;
static char s_msg[128];
static int  s_msg_frames = 0;
static int  s_battle_escaped = 0;
static int  s_battle_caught = 0;
static int  s_move_menu = 0;    /* 0 = main menu, 1 = move submenu */

static void battle_action(Game *g, int action)
{
    switch (action) {
    case 0: { /* FIGHT — open move submenu */
        Pokemon *pk = &g->player.team[0];
        int ms = 0;
        for (int i = 0; i < pk->move_count; i++)
            if (pk->moves[i].pp > 0) ms++;

        if (ms == 0) {
            snprintf(s_msg, sizeof(s_msg), "No moves with PP!");
            s_msg_frames = 90; s_move_phase = 1; return;
        }

        s_move_menu = 1; /* open move submenu */
        return;
    }
    case 1: { /* CATCH */
        if (g->player.pokeballs <= 0) {
            snprintf(s_msg, sizeof(s_msg), "No Pokeballs!");
            s_msg_frames = 90; s_move_phase = 1; return;
        }
        if (g->player.team_size >= MAX_TEAM) {
            snprintf(s_msg, sizeof(s_msg), "Team is full!");
            s_msg_frames = 90; s_move_phase = 1; return;
        }
        g->player.pokeballs--;
        if (battle_try_catch(&g->wild_pokemon, g->player.pokeballs + 1)) {
            player_add_pokemon(&g->player, g->wild_pokemon);
            snprintf(s_msg, sizeof(s_msg), "Gotcha! %s was caught!", g->wild_pokemon.name);
            s_battle_caught = 1;
        } else {
            snprintf(s_msg, sizeof(s_msg), "Oh no! It broke free!");
        }
        s_msg_frames = 120; s_move_phase = 1;
        break;
    }
    case 2: { /* RUN */
        if (rand() % 2 == 0) {
            snprintf(s_msg, sizeof(s_msg), "Got away safely!");
            s_battle_escaped = 1;
        } else {
            snprintf(s_msg, sizeof(s_msg), "Can't escape!");
        }
        s_msg_frames = 90; s_move_phase = 1;
        break;
    }
    }
}

static void battle_action_move(Game *g, int move_idx)
{
    Pokemon *pk = &g->player.team[0];
    if (move_idx < 0 || move_idx >= pk->move_count) return;
    if (pk->moves[move_idx].pp <= 0) {
        snprintf(s_msg, sizeof(s_msg), "No PP left for %s!", pk->moves[move_idx].name);
        s_msg_frames = 90; s_move_phase = 1; return;
    }

    pk->moves[move_idx].pp--;
    int dmg = battle_calculate_damage(pk, &g->wild_pokemon, &pk->moves[move_idx]);
    g->wild_pokemon.hp -= dmg;
    if (g->wild_pokemon.hp < 0) g->wild_pokemon.hp = 0;
    float eff = type_effectiveness(pk->moves[move_idx].type, g->wild_pokemon.type);
    const char *msg = (eff > 1.5f) ? "Super effective!" :
                      (eff < 0.5f && eff > 0.0f) ? "Not very effective..." : "";
    snprintf(s_msg, sizeof(s_msg), "%s used %s! %s", pk->name, pk->moves[move_idx].name, msg);
    s_msg_frames = 120; s_move_phase = 1; s_move_menu = 0;
}

static void update_battle(Game *g)
{
    if (s_move_phase == 0) {
        if (s_move_menu == 0) {
            /* main menu: 1=FIGHT, 2=CATCH, 3=RUN */
            if (IsKeyPressed(KEY_ONE))   battle_action(g, 0);
            if (IsKeyPressed(KEY_TWO))   battle_action(g, 1);
            if (IsKeyPressed(KEY_THREE)) battle_action(g, 2);
        } else {
            /* move submenu: 1/2/3 select move, 0=Back */
            if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_BACKSPACE)) {
                s_move_menu = 0;
            } else {
                Pokemon *pk = &g->player.team[0];
                int idx = -1;
                if (IsKeyPressed(KEY_ONE))   idx = 0;
                if (IsKeyPressed(KEY_TWO))   idx = 1;
                if (IsKeyPressed(KEY_THREE)) idx = 2;
                if (idx >= 0 && idx < pk->move_count && pk->moves[idx].pp > 0) {
                    battle_action_move(g, idx);
                }
            }
        }
    } else if (s_move_phase == 1) {
        if (s_msg_frames > 0) { s_msg_frames--; return; }
        if (s_battle_escaped || s_battle_caught) { s_move_phase = 3; return; }
        if (pokemon_is_fainted(&g->wild_pokemon))  { s_move_phase = 3; return; }
        if (pokemon_is_fainted(&g->player.team[0])) { s_move_phase = 3; return; }

        Move *em = battle_enemy_pick_move(&g->wild_pokemon);
        if (em) {
            em->pp--;
            int d = battle_calculate_damage(&g->wild_pokemon, &g->player.team[0], em);
            g->player.team[0].hp -= d;
            if (g->player.team[0].hp < 0) g->player.team[0].hp = 0;
            float eff = type_effectiveness(em->type, g->player.team[0].type);
            const char *msg = (eff > 1.5f) ? "Super effective!" :
                              (eff < 0.5f && eff > 0.0f) ? "Not very effective..." : "";
            snprintf(s_msg, sizeof(s_msg), "%s used %s! %s", g->wild_pokemon.name, em->name, msg);
        } else {
            snprintf(s_msg, sizeof(s_msg), "%s has no moves!", g->wild_pokemon.name);
        }
        s_msg_frames = 120; s_move_phase = 2;
    } else if (s_move_phase == 2) {
        if (s_msg_frames > 0) { s_msg_frames--; return; }
        s_move_phase = 0; s_move_menu = 0;
    } else {
        /* phase 3: end of battle */
        if (s_battle_escaped || s_battle_caught) {
            if (g->is_gym_battle && !s_battle_escaped && s_battle_caught) {
                /* caught in gym - rare but handle it */
                g->is_gym_battle = 0;
            }
            g->state = STATE_EXPLORE;
            s_move_phase = 0; s_battle_escaped = 0; s_battle_caught = 0;
            return;
        }

        if (pokemon_is_fainted(&g->wild_pokemon)) {
            int xp = g->player.team[0].level * 15;
            pokemon_gain_xp(&g->player.team[0], xp);
            snprintf(s_msg, sizeof(s_msg), "%s fainted! +%d XP",
                     g->wild_pokemon.name, xp);
        } else if (pokemon_is_fainted(&g->player.team[0])) {
            snprintf(s_msg, sizeof(s_msg), "%s fainted!",
                     g->player.team[0].name);
        }

        if (g->is_gym_battle) {
            g->gym_current++;
            while (g->gym_current < g->gym_team_size &&
                   pokemon_is_fainted(&g->gym_team[g->gym_current]))
                g->gym_current++;
            if (g->gym_current < g->gym_team_size) {
                g->wild_pokemon = g->gym_team[g->gym_current];
                g->state = STATE_BATTLE;
                s_move_phase = 0; s_sel = 0; s_move_menu = 0;
                return;
            }
            g->current_map.gym_defeated = 1;
            g->player.badges++;
            g->is_gym_battle = 0;
            strncat(s_msg, " | GYM DEFEATED! Badge earned!", sizeof(s_msg) - strlen(s_msg) - 1);
        }

        s_msg_frames = 150;
        s_move_phase = 4;
    }
}

static Game *g_ptr = NULL;

static void draw_battle_msg(void)
{
    /* text box at bottom */
    DrawRectangle(0, 400, VIEW_W, 200, (Color){20, 20, 40, 240});
    DrawRectangle(0, 400, VIEW_W, 3, C_YELLOW);

    if (s_move_phase == 0 && s_move_menu == 0) {
        /* main battle menu */
        draw_text_shadow(25, 415, "What will you do?", 20, C_WHITE);
        const char *opts[] = {"[1] FIGHT", "[2] CATCH", "[3] RUN"};
        for (int i = 0; i < 3; i++) {
            draw_text_shadow(25 + i * 250, 455, opts[i], 20, C_WHITE);
        }
        draw_text_shadow(25, 490, "Choose an action", 14, C_LIGHTGRAY);
    } else if (s_move_phase == 0 && s_move_menu == 1) {
        /* move submenu */
        draw_text_shadow(25, 415, "Choose a move:", 20, C_WHITE);
        Pokemon *pk = &g_ptr->player.team[0];
        for (int i = 0; i < pk->move_count && i < 3; i++) {
            Color mc = (pk->moves[i].pp > 0) ? C_WHITE : C_DARKGRAY;
            char buf[64];
            snprintf(buf, sizeof(buf), "[%d] %s  PWR:%d  PP:%d/%d",
                     i + 1, pk->moves[i].name,
                     pk->moves[i].power,
                     pk->moves[i].pp,
                     pk->moves[i].max_pp);
            draw_text_shadow(25, 445 + i * 25, buf, 18, mc);
        }
        draw_text_shadow(25, 530, "[0] Back", 14, C_LIGHTGRAY);
    } else if (s_move_phase == 1 || s_move_phase == 2 || s_move_phase == 4) {
        /* battle message */
        draw_text_shadow(25, 420, s_msg, 18, C_WHITE);
        draw_text_shadow(25, 470, "[ENTER] Continue", 14, C_LIGHTGRAY);
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            if (s_move_phase == 4) {
                s_move_phase = 0; s_sel = 0; s_move_menu = 0;
                if (pokemon_is_fainted(&g_ptr->player.team[0]) && player_all_fainted(&g_ptr->player))
                    g_ptr->state = STATE_GAME_OVER;
                else
                    g_ptr->state = STATE_EXPLORE;
            } else {
                s_move_phase++;
            }
        }
    }
}

/* ---- main entry ---- */

void graphics_init(void)
{
    InitWindow(VIEW_W, VIEW_H, "Pokeshell");
    SetTargetFPS(60);
}

void graphics_run(void)
{
    Game game;
    memset(&game, 0, sizeof(Game));
    game.state = STATE_TITLE;
    game.current_map = world_create_starting_map();
    game.player = player_create("Ash");
    game.player.pos_x = 2;
    game.player.pos_y = 2;
    srand((unsigned int)time(NULL));

    g_ptr = &game;

    while (!WindowShouldClose()) {
        switch (game.state) {
        case STATE_TITLE:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                game.state = STATE_STARTER_SELECT;
            }
            draw_title();
            break;

        case STATE_STARTER_SELECT: {
            int c = draw_starter_select();
            if (c >= 0) {
                const char *n[] = {"Bulbasaur","Charmander","Squirtle"};
                Type        t[] = {TYPE_GRASS,  TYPE_FIRE,    TYPE_WATER};
                int hp[]        = {45, 39, 44};
                int at[]        = {49, 52, 48};
                int df[]        = {49, 43, 65};
                Pokemon s = pokemon_create_starter(n[c], t[c], hp[c], at[c], df[c]);
                player_add_pokemon(&game.player, s);

                /* starter moves already assigned by pokemon_create_starter */
                game.state = STATE_EXPLORE;
            }
            break;
        }

        case STATE_EXPLORE:
            update_explore(&game);
            draw_explore(&game);
            break;

        case STATE_BATTLE:
            update_battle(&game);
            draw_battle(&game);
            if (s_move_phase == 0) draw_battle_msg();
            else if (s_move_phase >= 1) draw_battle_msg();
            break;

        case STATE_GAME_OVER:
            ClearBackground(C_BLACK);
            draw_text_shadow(230, 220, "GAME OVER", 50, C_RED);
            draw_text_shadow(250, 300, "[ENTER] Exit", 22, C_WHITE);
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
                game.state = (GameState)99;
            break;

        default:
            break;
        }

        EndDrawing();
    }

    CloseWindow();
}
