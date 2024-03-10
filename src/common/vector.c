#include <stdio.h>
#include <string.h>
#include <math.h>

#include "common/logging.h"
#include "common/vector.h"
#include "common/fixed.h"

vector_t vector_create(fixed_t x, fixed_t y)
{
    vector_t v;
    v.x = x;
    v.y = y;
    return v;
}

vector_t vector_add(vector_t v1, vector_t v2)
{
    vector_t result;
    result.x = fixed_add(v1.x, v2.x);
    result.y = fixed_add(v1.y, v2.y);
    return result;
}

vector_t vector_subtract(vector_t v1, vector_t v2)
{
    vector_t result;
    result.x = fixed_sub(v1.x, v2.x);
    result.y = fixed_sub(v1.y, v2.y);
    return result;
}

fixed_t vector_dot_product(vector_t v1, vector_t v2)
{
    return fixed_mul(v1.x, v2.x) + fixed_mul(v1.y, v2.y);
}

fixed_t vector_magnitude(vector_t v)
{
    return fixed_sqrt(fixed_mul(v.x, v.x) + fixed_mul(v.y, v.y));
}

vector_t vector_normalize(vector_t v)
{
    fixed_t magnitude = vector_magnitude(v);
    vector_t result;
    result.x = fixed_div(v.x, magnitude);
    result.y = fixed_div(v.y, magnitude);
    return result;
}

vector_t vector_multiply(vector_t v, fixed_t scalar)
{
    vector_t result;
    result.x = fixed_mul(v.x, scalar);
    result.y = fixed_mul(v.y, scalar);
    return result;
}

void vector_print(vector_t v)
{
    char x_buffer[64];
    char y_buffer[64];
    fixed_to_string(v.x, x_buffer);
    fixed_to_string(v.y, y_buffer);

    printf("(%s, %s)\n", x_buffer, y_buffer);
}

void vector_to_string(vector_t v, char *buffer)
{
    char x_buffer[64];
    char y_buffer[64];
    fixed_to_string(v.x, x_buffer);
    fixed_to_string(v.y, y_buffer);

    sprintf(buffer, "(%s, %s)", x_buffer, y_buffer);
}

void vector_copy(vector_t *dest, vector_t *src)
{
    dest->x = src->x;
    dest->y = src->y;
}

bool vector_serialize(vector_t *vector, char *buffer, uint32_t *buffer_size)
{
    if (vector == NULL || buffer == NULL || buffer_size == NULL)
    {
        log_error("Invalid arguments");
        return false;
    }

    uint32_t required_size = sizeof(vector_t);
    if (*buffer_size < required_size)
    {
        log_error("Buffer too small");
        return false;
    }

    fixed_serialize(vector->x, buffer, *buffer_size);
    buffer += sizeof(fixed_t);
    *buffer_size -= sizeof(fixed_t);
    fixed_serialize(vector->y, buffer, *buffer_size);

    return true;
}

bool vector_deserialize(char *buffer, uint32_t buffer_size, vector_t *vector)
{
    if (buffer == NULL || vector == NULL)
    {
        log_error("Invalid arguments");
        return false;
    }

    if (buffer_size < sizeof(vector_t))
    {
        log_error("Buffer too small");
        return false;
    }

    fixed_deserialize(buffer, buffer_size, &vector->x);
    buffer += sizeof(fixed_t);
    buffer_size -= sizeof(fixed_t);
    fixed_deserialize(buffer, buffer_size, &vector->y);

    return true;
}