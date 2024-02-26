#include <stdio.h>
#include <stdlib.h>
#include <common/entity.h>

Entity entity_create(Vector2D position, Vector2D velocity, int radius)
{
    Entity e;
    e.position = position;
    e.velocity = velocity;
    e.radius = radius;
    return e;
}
