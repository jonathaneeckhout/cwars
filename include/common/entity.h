#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>

#include "common/vector.h"

typedef struct
{
    vector_t position;
    vector_t velocity;
    int radius;
} entity_t;

entity_t *entity_init(vector_t position, vector_t velocity, int radius);
void entity_cleanup(entity_t **entity);
void entity_update(entity_t *entity, int64_t delta_time);

#endif // ENTITY_H
