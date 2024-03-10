#ifndef ENTITY_H
#define ENTITY_H

#include <stdint.h>
#include <stdbool.h>

#include "common/vector.h"

typedef struct
{
    char id[37];
    vector_t position;
    vector_t velocity;
    uint32_t radius;
} entity_t;

entity_t *entity_init(vector_t position, vector_t velocity, uint32_t radius);
void entity_cleanup(entity_t **entity);

bool entity_serialize(entity_t *entity, char *buffer, uint32_t *buffer_size);
bool entity_deserialize(char *buffer, uint32_t buffer_size, entity_t *entity);

#endif // ENTITY_H
