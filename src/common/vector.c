#include "common/vector.h"
#include <math.h>

Vector2D vector_create(float x, float y)
{
    Vector2D v;
    v.x = x;
    v.y = y;
    return v;
}

Vector2D vector_add(Vector2D v1, Vector2D v2)
{
    Vector2D result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    return result;
}

Vector2D vector_subtract(Vector2D v1, Vector2D v2)
{
    Vector2D result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    return result;
}

float vector_dot_product(Vector2D v1, Vector2D v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

float vector_magnitude(Vector2D v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}

Vector2D vector_normalize(Vector2D v)
{
    float magnitude = vector_magnitude(v);
    Vector2D result;
    result.x = v.x / magnitude;
    result.y = v.y / magnitude;
    return result;
}

Vector2D vector_multiply(Vector2D v, float scalar)
{
    Vector2D result;
    result.x = v.x * scalar;
    result.y = v.y * scalar;
    return result;
}