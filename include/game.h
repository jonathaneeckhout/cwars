#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "server.h"

typedef struct
{
    bool running;
    Server *server;
} Game;

Game *game_init();
void game_cleanup(Game **game);

void game_run(Game *game);
void game_stop(Game *game);

#endif /* GAME_H */