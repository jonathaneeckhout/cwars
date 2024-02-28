#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "client/client.h"

typedef struct
{
    client_t *client;
} game_t;

game_t *game_init();
void game_cleanup(game_t **game);

void game_loop_once(game_t *game);

#endif /* GAME_H */