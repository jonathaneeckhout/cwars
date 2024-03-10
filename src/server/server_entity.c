#include "common/utils.h"
#include "server/server_entity.h"

void entity_update(entity_t UNUSED *entity, game_t UNUSED *game, int64_t UNUSED delta_time)
{
    // entity->position.x += entity->velocity.x * delta_time / 1000.0;
    // if (entity->position.x < 0)
    // {
    //     entity->position.x = 0;
    //     entity->velocity.x = -entity->velocity.x;
    // }
    // else if (entity->position.x > game->map->width)
    // {
    //     entity->position.x = game->map->width;
    //     entity->velocity.x = -entity->velocity.x;
    // }
    // entity->position.y += entity->velocity.y * delta_time / 1000.0;
    // if (entity->position.y < 0)
    // {
    //     entity->position.y = 0;
    //     entity->velocity.y = -entity->velocity.y;
    // }
    // else if (entity->position.y > game->map->height)
    // {
    //     entity->position.y = game->map->height;
    //     entity->velocity.y = -entity->velocity.y;
    // }
}