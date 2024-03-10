#include <stdio.h>
#include <stdlib.h>
#include <uuid/uuid.h>

#include "common/entity.h"

entity_t *entity_init(vector_t position, vector_t velocity, uint32_t radius)
{
    uuid_t bin_id;
    entity_t *entity = calloc(1, sizeof(entity_t));
    if (entity == NULL)
    {
        return NULL;
    }

    uuid_generate(bin_id);
    uuid_unparse(bin_id, entity->id);

    vector_copy(&entity->position, &position);
    vector_copy(&entity->velocity, &velocity);

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
