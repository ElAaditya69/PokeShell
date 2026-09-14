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
