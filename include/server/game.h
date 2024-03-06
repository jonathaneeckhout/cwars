#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#include "common/map.h"

#include "server/server.h"

typedef struct
{
    bool running;
    server_t *server;
    map_t *map;
} game_t;

game_t *game_init();
void game_cleanup(game_t **game);

void game_run(game_t *game);
void game_stop(game_t *game);

#endif /* GAME_H */