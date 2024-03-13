#include "common/utils.h"
#include "server/server_entity.h"

void entity_update(entity_t UNUSED *entity, game_t UNUSED *game, int64_t UNUSED delta_time)
{
    entity->position.x = fixed_add(entity->position.x, fixed_mul(entity->velocity.x, fixed_from_int(delta_time)));
    if (entity->position.x < 0)
    {
        entity->position.x = 0;
        entity->velocity.x = -entity->velocity.x;
    }
    else if (entity->position.x > fixed_from_int(game->map->width))
    {
        entity->position.x = fixed_from_int(game->map->width);
        entity->velocity.x = -entity->velocity.x;
    }

    entity->position.y = fixed_add(entity->position.y, fixed_mul(entity->velocity.y, fixed_from_int(delta_time)));
    if (entity->position.y < 0)
    {
        entity->position.y = 0;
        entity->velocity.y = -entity->velocity.y;
    }
    else if (entity->position.y > fixed_from_int(game->map->height))
    {
        entity->position.y = fixed_from_int(game->map->height);
        entity->velocity.y = -entity->velocity.y;
    }
}