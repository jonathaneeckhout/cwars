#ifndef GAME_H
#define GAME_H

#include "server/types.h"

game_t *game_init();
void game_cleanup(game_t **game);

void game_run(game_t *game);
void game_stop(game_t *game);

#endif /* GAME_H */