#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#include "common/ctimer.h"
#include "client/client.h"

typedef struct
{
    client_t *client;
    ctimer_t *ping_timer;

} game_t;

game_t *game_init();
void game_cleanup(game_t **game);

void game_loop_once(game_t *game, long delta_time);

#endif /* GAME_H */