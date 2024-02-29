#ifndef ENTITY_H
#define ENTITY_H

#include <common/vector.h>

typedef struct
{
    Vector2D position;
    Vector2D velocity;
    int radius;
} Entity;

#endif // ENTITY_H