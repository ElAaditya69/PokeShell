# PokeShell — My Project Plan

So I want to make a Pokemon game that runs in the terminal. Turn-based, text-based, no graphics — just pure terminal vibes. Pick a starter, walk around, encounter wild Pokemon, battle them, catch them, level up, beat gyms, become champion. The whole deal, but in a terminal.

I'm writing this in C because I want to actually get better at C. And this time I'm doing it right — planning everything before I write a single line of code. No more mess.

**Language:** C (C99 standard)
**Build system:** Makefile (so I just type `make` and it compiles)
**Colors:** ANSI escape codes in the terminal
**Version control:** Git, will push to GitHub

---

## How I'm Organizing the Project

```
pokeshell/
├── Makefile              # type `make` to build, `make run` to play
├── README.md             # what the game is, how to build it
├── DESIGN.md             # game design stuff — types, stats, formulas
├── .gitignore            # ignore .o files, the executable, editor junk
│
├── include/              # header files — all my struct and function declarations go here
│   ├── game.h            # game state machine, the main Game struct
│   ├── pokemon.h         # Pokemon struct, how to create one, how leveling works
│   ├── move.h            # Move struct, the move database
│   ├── battle.h          # battle logic — damage calc, turn system
│   ├── types.h           # type effectiveness (fire beats grass, etc.)
│   ├── player.h          # player struct — team, pokeballs, position
│   ├── world.h           # maps, tiles, movement, encounter zones
│   ├── ui.h              # terminal display — menus, HP bars, input
│   └── save.h            # save/load game to a file
│
├── src/                  # the actual code
│   ├── main.c            # entry point, starts everything up
│   ├── game.c            # state machine — switches between explore, battle, menu
│   ├── pokemon.c         # creating pokemon, stat calc, leveling up
│   ├── move.c            # loading moves from the data file
│   ├── battle.c          # the whole battle engine
│   ├── types.c           # type effectiveness lookup table
│   ├── player.c          # player stuff — team management, pokeballs
│   ├── world.c           # maps, walking, random encounters
│   ├── ui.c              # all the terminal display stuff
│   └── save.c            # reading/writing save files
│
├── data/                 # game data I can edit without recompiling
│   ├── pokemon.txt       # all pokemon species and their base stats
│   ├── moves.txt         # all moves — name, type, power, accuracy
│   └── maps.txt          # map layouts (routes, towns, gyms)
│
└── saves/                # save files go here (created when you first save)
    └── .gitkeep          # empty file so git keeps the folder
```

The idea is: `include/` has declarations, `src/` has implementations, `data/` has game data. Each .c file does one thing. If it's about battles, it goes in `battle.c`. If it's about display, it goes in `ui.c`. Simple.

---

## What Each File Does (the short version)

**Headers (include/)**

- `game.h` — game states like EXPLORE, BATTLE, MENU, GAME_OVER. The big Game struct that holds everything together.
- `pokemon.h` — Pokemon struct with name, type, level, HP, attack, defense, moves. Also the species data for base stats.
- `move.h` — Move struct with name, type, power, accuracy, PP.
- `battle.h` — damage formula, turn management, catch logic, XP after battle.
- `types.h` — Type enum (FIRE, WATER, GRASS, etc.) and a function that returns the effectiveness multiplier.
- `player.h` — Player struct — name, team of up to 6 Pokemon, pokeball count, badges, map position.
- `world.h` — Map struct — 2D grid of tiles (grass, path, wall, water, town), encounter rates.
- `ui.h` — display functions, input handling, HP bars, menus.
- `save.h` — save game to file, load game from file.

**Source (src/)**

- `main.c` — sets up terminal, creates initial game state, runs the main loop, cleans up on exit.
- `game.c` — the state machine. EXPLORE calls world functions, BATTLE calls battle functions, MENU calls ui functions. Keeps everything connected.
- `pokemon.c` — `pokemon_create()` to make one from species data, `pokemon_gain_xp()` to level up, stat formulas.
- `move.c` — loads moves from `data/moves.txt`, finds a move by name.
- `battle.c` — the full battle flow: player picks move or catch, enemy AI picks move, damage applied, check for faints, loop until done.
- `types.c` — 2D array of effectiveness values. 2.0 for super effective, 0.5 for not very effective, 0.0 for immune.
- `player.c` — add/remove Pokemon from team, use pokeballs, check if all Pokemon fainted.
- `world.c` — loads maps from `data/maps.txt`, handles WASD movement, checks if stepping on grass triggers encounter.
- `ui.c` — ANSI colors, clearing screen, drawing HP bars with unicode blocks, reading single keypresses.
- `save.c` — writes game state to a text file, reads it back.

**Data (data/)**

- `pokemon.txt` — one line per species: `name type base_hp base_atk base_def moves_at_levels`
- `moves.txt` — one line per move: `name type power accuracy pp`
- `maps.txt` — grids of characters: `#` wall, `.` path, `~` grass, `W` water, `T` town

---

## The Structs I Need

These are the core data types everything else is built on.

```c
// the types — keeping it simple for now
typedef enum {
    TYPE_NORMAL, TYPE_FIRE, TYPE_WATER,
    TYPE_GRASS, TYPE_ELECTRIC, TYPE_ICE,
    TYPE_COUNT
} Type;

// a move a pokemon can use
typedef struct {
    char name[32];
    Type type;
    int power;
    int accuracy;    // out of 100
    int pp;          // how many times i can use it
    int max_pp;
} Move;

// an actual pokemon instance (not a species, but the one i caught)
typedef struct {
    char name[32];
    Type type;
    int level;
    int hp;
    int max_hp;
    int attack;
    int defense;
    int xp;
    int xp_to_next;
    Move moves[4];       // max 4 moves like real pokemon
    int move_count;
} Pokemon;

// the player
typedef struct {
    char name[32];
    Pokemon team[6];     // max 6 pokemon in team
    int team_size;
    int pokeballs;
    int badges;
    int pos_x;
    int pos_y;
} Player;

// map tiles
typedef enum {
    TILE_PATH, TILE_GRASS, TILE_WATER, TILE_WALL, TILE_TOWN
} TileType;

// a map
typedef struct {
    TileType tiles[20][20];   // 20x20 grid should be enough
    int width;
    int height;
    int encounter_rate;       // 1 in N steps triggers encounter in grass
} Map;

// game states
typedef enum {
    STATE_TITLE, STATE_STARTER_SELECT,
    STATE_EXPLORE, STATE_BATTLE,
    STATE_MENU, STATE_GAME_OVER, STATE_WIN
} GameState;

// everything about the current game
typedef struct {
    GameState state;
    Player player;
    Map current_map;
    Pokemon wild_pokemon;     // the wild pokemon currently in battle
    int battle_turn;          // 0 = player, 1 = enemy
} Game;
```

---

## The Makefile

This took me a sec to understand but it's actually simple. It's basically a recipe that says "compile all these .c files and link them together."

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
SRC = src/main.c src/game.c src/pokemon.c src/move.c \
      src/battle.c src/types.c src/player.c src/world.c \
      src/ui.c src/save.c
OBJ = $(SRC:.c=.o)
TARGET = pokeshell

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
```

- `make` compiles everything into the `pokeshell` executable
- `make run` compiles and runs it
- `make clean` deletes all the .o files and the executable
- when I add a new .c file I just add it to the SRC line

---

## Rules I'm Following (so it stays organized)

- **snake_case everywhere.** Structs are Capitalized though: `Pokemon`, `Move`, `Player`. Functions are `pokemon_create()`, `battle_calculate_damage()`, `ui_print_hp_bar()`.
- **Header guards on every .h file.** `#ifndef POKESHELL_POKEMON_H` / `#define` / `#endif`. Every time.
- **Function names start with the file name.** Everything in `battle.c` starts with `battle_`. Everything in `ui.c` starts with `ui_`. That way when I see `pokemon_calc_stats()` I know exactly where to find it.
- **One job per file.** If it draws stuff on screen, it goes in `ui.c`. If it's damage math, it goes in `battle.c`. Don't mix things up.
- **No magic numbers.** `#define MAX_TEAM_SIZE 6` not just putting `6` everywhere. Future me will thank present me.
- **Keep functions short.** Under 50 lines ideally. If it's getting long, break it into smaller functions.
- **Comment the why, not the what.** Don't write `// increment i`. Do write `// wild pokemon level depends on how far along the route you are`.

---

## Milestones — the Plan

### Milestone 1 — Foundation (Days 1-2)

Get the skeleton working. Everything compiles, structs are defined, type system works.

- [x] Set up the whole folder structure — all folders, all empty files with header guards
- [x] `git init`, make `.gitignore`, first commit: "initial project structure"
- [x] Write the Makefile, verify `make` compiles an empty main.c with no errors
- [x] Write out all the structs in the header files
- [x] Implement `types.c` — the type effectiveness table
- [x] Implement `pokemon.c` — create a pokemon from species data, calc stats at a level
- [x] Quick test in main.c — create a pokemon, print its stats, make sure it works
- [ ] Commit: "core data structures and type system done"

### Milestone 2 — Battle Engine (Days 3-5)

The fun part. Two Pokemon fight each other and it actually works.

- [ ] Implement `move.c` — load moves from data/moves.txt
- [ ] Implement `battle.c` — damage formula, turn loop, someone faints
- [ ] Implement `ui.c` — HP bar display, battle menu, move selection
- [ ] Implement `player.c` — team management, switching pokemon
- [ ] Create `data/pokemon.txt` and `data/moves.txt` with the 3 starters and their moves
- [ ] Test it — manually make two pokemon fight from main.c
- [ ] Commit: "battle engine works"

### Milestone 3 — Exploration (Days 6-8)

Walking around the map and finding wild pokemon.

- [ ] Implement `world.c` — load a map, handle WASD movement
- [ ] Create `data/maps.txt` — a town, Route 1, and a gym
- [ ] Connect exploration to battle — step on grass, random chance of encounter
- [ ] Wild pokemon generation — random species, level based on route
- [ ] Catch mechanic — throw pokeball, calculate catch rate
- [ ] Commit: "can walk around and find wild pokemon"

### Milestone 4 — Full Game Loop (Days 9-11)

The whole game from start to finish.

- [ ] Implement `game.c` — state machine that connects everything
- [ ] Starter selection screen — pick from 3 pokemon
- [ ] Level up system — gain XP, stats increase, learn new moves at levels
- [ ] Evolution — pokemon evolve at certain levels
- [ ] Title screen, menu, game over screen in ui.c
- [ ] Save and load with save.c
- [ ] Commit: "full game loop working"

### Milestone 5 — Polish (Days 12-14)

Make it actually fun to play.

- [ ] ANSI colors on everything
- [ ] Simple ASCII art for pokemon in battle
- [ ] Balance encounter rates, enemy levels, XP curves
- [ ] Pokemon Center — heal your team
- [ ] 2-3 gyms with gym leaders as boss fights
- [ ] Write README.md with build instructions and controls
- [ ] Final commit: "pokeshell v1.0 done"

---

## Game Flow

```
START
  |
  v
[Title Screen] "POKESHELL" -- press ENTER
  |
  v
[Starter Pick] Bulbasaur / Charmander / Squirtle
  |
  v
[Town] heal pokemon, buy pokeballs, walk to route
  |
  v
[Route 1] walk around -- step in grass -- random encounter
  |                                            |
  |                                            v
  |                                   [Battle Mode]
  |                                   Fight / Catch / Run
  |                                            |
  |                            +---------+-----------+---------+
  |                            v         v                     v
  |                       [Faint]   [Caught!]            [Ran Away]
  |                         |           |                     |
  |                         v           v                     v
  |                     gain XP    add to team          back to route
  |                         |
  |                         v
  |                  [Level Up?] --> learn new move? evolve?
  |
  v
[Route 2] --> [Gym 1] --> [Gym Leader Battle]
  |                              |
  |                              v
  |                         [Badge Earned]
  |
  v
[Route 3] --> [Gym 2] --> ... --> [Pokemon League]
                                       |
                                       v
                                 [Champion Battle]
                                       |
                                       v
                                   [YOU WIN!]
```

---

## Save File Format

Keeping it simple text so I can debug it easily if something breaks:

```
PLAYER_NAME Red
POKEBALLS 10
BADGES 2
POSITION 5 3
TEAM_SIZE 3
POKEMON Charmander FIRE 18 45 45 22 18 Tackle NORMAL 40 100 25 Ember FIRE 40 100 15
POKEMON Pidgey NORMAL 12 30 30 15 12 Gust NORMAL 40 100 35
POKEMON Pikachu ELECTRIC 15 38 38 20 15 ThunderShock ELECTRIC 40 100 30
MAP current_route_1
```

---

## What I'm Doing First

Right now, before writing any actual game code:

1. ~~Create the full folder structure on my machine at `~/Documents/pokeshell`~~
2. ~~`git init` and create `.gitignore`~~
3. ~~Write the Makefile~~
4. ~~Create all header and source files as empty stubs (just header guards in .h, empty main.c)~~
5. ~~Make sure `make` compiles without errors~~
6. `git add . && git commit -m "initial project structure"`

That's the foundation. Nothing works yet, but everything is in place and compiling. Then I build from there, milestone by milestone.
