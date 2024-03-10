#ifndef FIXED_H
#define FIXED_H

#include <stdint.h>
#include <stdbool.h>

typedef int64_t fixed_t;

fixed_t fixed_from_int(int32_t i);
fixed_t fixed_from_float(float f);
int32_t fixed_to_int(fixed_t x);
float fixed_to_float(fixed_t x);

fixed_t fixed_add(fixed_t x, fixed_t y);
fixed_t fixed_sub(fixed_t x, fixed_t y);
fixed_t fixed_mul(fixed_t x, fixed_t y);
fixed_t fixed_div(fixed_t x, fixed_t y);
fixed_t fixed_sqrt(fixed_t x);

void fixed_print(fixed_t f);
void fixed_to_string(fixed_t f, char *buffer);

bool fixed_serialize(fixed_t f, char *buffer, uint32_t buffer_size);
bool fixed_deserialize(char *buffer, uint32_t buffer_size, fixed_t *f);

#endif // FIXED_H