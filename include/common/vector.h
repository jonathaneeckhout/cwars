#ifndef VECTOR_H
#define VECTOR_H

typedef struct
{
    float x;
    float y;
} vector_t;

vector_t vector_create(float x, float y);
vector_t vector_add(vector_t v1, vector_t v2);
vector_t vector_subtract(vector_t v1, vector_t v2);
vector_t vector_multiply(vector_t v, float scalar);
float vector_dot_product(vector_t v1, vector_t v2);
float vector_magnitude(vector_t v);
vector_t vector_normalize(vector_t v);

#endif // VECTOR_H
