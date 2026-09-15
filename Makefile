CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-variable -Wno-unused-function -std=c99 -Iinclude -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lraylib -framework OpenGL -framework IOKit -framework CoreFoundation -framework CoreVideo
SRC = src/main.c src/game.c src/pokemon.c src/move.c \
      src/battle.c src/types.c src/player.c src/world.c \
      src/ui.c src/save.c src/graphics.c
OBJ = $(SRC:.c=.o)
TARGET = pokeshell

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
