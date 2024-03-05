#include "common/vector.h"
#include <math.h>

vector_t vector_create(float x, float y)
{
    vector_t v;
    v.x = x;
    v.y = y;
    return v;
}

vector_t vector_add(vector_t v1, vector_t v2)
{
    vector_t result;
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    return result;
}

vector_t vector_subtract(vector_t v1, vector_t v2)
{
    vector_t result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    return result;
}

float vector_dot_product(vector_t v1, vector_t v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

float vector_magnitude(vector_t v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}

vector_t vector_normalize(vector_t v)
{
    float magnitude = vector_magnitude(v);
    vector_t result;
    result.x = v.x / magnitude;
    result.y = v.y / magnitude;
    return result;
}

vector_t vector_multiply(vector_t v, float scalar)
{
    vector_t result;
    result.x = v.x * scalar;
    result.y = v.y * scalar;
    return result;
}