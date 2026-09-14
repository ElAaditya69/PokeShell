# Pokeshell

A Pokemon game that runs in your terminal. Written in C for fun and to get better at C.

Pick your starter, walk around, battle wild Pokemon, catch them, level up, beat gyms, become champion. All in text.

## How to build

```bash
make
```

## How to play

```bash
make run
```

Or just run the executable directly:

```bash
./pokeshell
```

## Controls

- **WASD** or **arrow keys** to move around the map
- **1-4** to select moves in battle
- **C** to throw a pokeball
- **R** to run from battle
- **M** to open the menu
- **ESC** to quit

## Project Structure

```
pokeshell/
├── include/    header files (structs, function declarations)
├── src/        source code (the actual game logic)
├── data/       game data (pokemon stats, moves, maps)
├── saves/      save files
├── Makefile    build rules
```

## Dependencies

- gcc
- make
- A terminal that supports ANSI escape codes (most do)
