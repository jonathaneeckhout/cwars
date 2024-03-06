#ifndef ENTITY_H
#define ENTITY_H

#include "common/vector.h"

typedef struct
{
    vector_t position;
    vector_t velocity;
    int radius;
} entity_t;

entity_t *entity_init(vector_t position, vector_t velocity, int radius);
void entity_cleanup(entity_t **entity);

#endif // ENTITY_H
