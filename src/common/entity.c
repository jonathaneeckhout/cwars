#include <stdio.h>
#include <stdlib.h>
#include <common/entity.h>

entity_t entity_create(vector_t position, vector_t velocity, int radius)
{
    entity_t e;
    e.position = position;
    e.velocity = velocity;
    e.radius = radius;
    return e;
}
