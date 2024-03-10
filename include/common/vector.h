#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stdbool.h>

#include "common/fixed.h"

typedef struct
{
    fixed_t x;
    fixed_t y;
} vector_t;

vector_t vector_create(fixed_t x, fixed_t y);
vector_t vector_add(vector_t v1, vector_t v2);
vector_t vector_subtract(vector_t v1, vector_t v2);
vector_t vector_multiply(vector_t v, fixed_t scalar);
fixed_t vector_dot_product(vector_t v1, vector_t v2);
fixed_t vector_magnitude(vector_t v);
vector_t vector_normalize(vector_t v);
void vector_copy(vector_t *dest, vector_t *src);

void vector_print(vector_t v);
void vector_to_string(vector_t v, char *buffer);

bool vector_serialize(vector_t *vector, char *buffer, uint32_t *buffer_size);
bool vector_deserialize(char *buffer, uint32_t buffer_size, vector_t *vector);

#endif // VECTOR_H
