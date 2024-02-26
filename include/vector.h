#ifndef VECTOR_H
#define VECTOR_H

typedef struct
{
    float x;
    float y;
} Vector2D;

Vector2D vector_create(float x, float y);
Vector2D vector_add(Vector2D v1, Vector2D v2);
Vector2D vector_subtract(Vector2D v1, Vector2D v2);
Vector2D vector_multiply(Vector2D v, float scalar);
float vector_dot_product(Vector2D v1, Vector2D v2);
float vector_magnitude(Vector2D v);
Vector2D vector_normalize(Vector2D v);

#endif // VECTOR_H
