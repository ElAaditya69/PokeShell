#include "game.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    srand((unsigned int)time(NULL));

    Game game;
    game_init(&game);
    game_run(&game);
    game_cleanup(&game);

    return 0;
}
