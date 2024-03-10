#ifndef SERVER_ENTITY_H
#define SERVER_ENTITY_H

#include "common/entity.h"

#include "server/types.h"

void entity_update(entity_t *entity, game_t *game, int64_t delta_time);

#endif // SERVER_ENTITY_H