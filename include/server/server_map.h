#ifndef SERVER_MAP_H
#define SERVER_MAP_H

#include "common/map.h"
#include "server/types.h"

void map_update(map_t *map, game_t *game, int64_t delta_time);

#endif // SERVER_MAP_H