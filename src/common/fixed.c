#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/fixed.h"
#include "common/utils.h"

#define FIXED_POINT 16

fixed_t fixed_from_int(int32_t i)
{
    fixed_t f = i << FIXED_POINT;
    return f;
}

fixed_t fixed_from_float(float x)
{
    fixed_t f = (int64_t)(x * (1 << FIXED_POINT));
    return f;
}

int32_t fixed_to_int(fixed_t f)
{
    return f >> FIXED_POINT;
}

float fixed_to_float(fixed_t f)
{
    return (float)f / (1 << FIXED_POINT);
}

fixed_t fixed_add(fixed_t a, fixed_t b)
{
    fixed_t f = a + b;
    return f;
}

fixed_t fixed_sub(fixed_t a, fixed_t b)
{
    fixed_t f = a - b;
    return f;
}

fixed_t fixed_mul(fixed_t a, fixed_t b)
{
    fixed_t f = (a * b) >> FIXED_POINT;
    return f;
}

fixed_t fixed_div(fixed_t a, fixed_t b)
{
    fixed_t f = (a << FIXED_POINT) / b;
    return f;
}
fixed_t fixed_sqrt(fixed_t x)
{
    fixed_t z = x;
    for (int i = 0; i < 10; i++)
    {
        z = (z + fixed_div(x, z)) >> 1;
    }
    return z;
}
void fixed_print(fixed_t f)
{
    printf("%ld.%04ld\n", f >> FIXED_POINT, (f & ((1 << FIXED_POINT) - 1)) * 10000 >> FIXED_POINT);
}

void fixed_to_string(fixed_t f, char *buffer)
{
    sprintf(buffer, "%ld.%04ld", f >> FIXED_POINT, (f & ((1 << FIXED_POINT) - 1)) * 10000 >> FIXED_POINT);
}

bool fixed_serialize(fixed_t f, char *buffer, uint32_t buffer_size)
{
    if (buffer == NULL || buffer_size < sizeof(int64_t))
    {
        return false;
    }

    fixed_t network_order = htonll(f);

    memcpy(buffer, &network_order, sizeof(fixed_t));
    return true;
}

bool fixed_deserialize(char *buffer, uint32_t buffer_size, fixed_t *f)
{
    if (buffer == NULL || buffer_size < sizeof(fixed_t) || f == NULL)
    {
        return false;
    }

    fixed_t network_order;
    memcpy(&network_order, buffer, sizeof(fixed_t));

    *f = ntohll(network_order);
    return true;
}
