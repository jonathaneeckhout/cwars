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
