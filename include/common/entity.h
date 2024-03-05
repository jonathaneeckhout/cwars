#ifndef ENTITY_H
#define ENTITY_H

#include <common/vector.h>

typedef struct
{
    vector_t position;
    vector_t velocity;
    int radius;
} entity_t;

#endif // ENTITY_H
