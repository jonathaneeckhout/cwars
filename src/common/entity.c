#include <stdio.h>
#include <stdlib.h>

#include "common/entity.h"

entity_t *entity_init(vector_t position, vector_t velocity, int radius)
{
    entity_t *entity = calloc(1, sizeof(entity_t));
    if (entity == NULL)
    {
        return NULL;
    }

    entity->position = position;
    entity->velocity = velocity;
    entity->radius = radius;

    return entity;
}

void entity_cleanup(entity_t **entity)
{
    if (entity == NULL || *entity == NULL)
    {
        return;
    }

    free(*entity);
    *entity = NULL;
}

void entity_update(entity_t *entity, int64_t delta_time)
{
    entity->position.x += entity->velocity.x * delta_time/1000.0;
    if (entity->position.x < 0)
    {
        entity->position.x = 0;
        entity->velocity.x = -entity->velocity.x;
    }
    else if (entity->position.x > 800)
    {
        entity->position.x = 800;
        entity->velocity.x = -entity->velocity.x;
    }
    entity->position.y += entity->velocity.y * delta_time / 1000.0;
    if (entity->position.y < 0)
    {
        entity->position.y = 0;
        entity->velocity.y = -entity->velocity.y;
    }
    else if (entity->position.y > 600)
    {
        entity->position.y = 600;
        entity->velocity.y = -entity->velocity.y;
    }
}
