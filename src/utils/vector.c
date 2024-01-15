#include "vector.h"

#include <string.h>
#include <stdlib.h>

struct vector *vector_create(size_t capacity)
{
    struct vector *new = calloc(1, sizeof(struct vector));
    if (new == NULL)
    {
        return NULL;
    }

    new->capacity = capacity;
    new->data = calloc(capacity, sizeof(sizeof(char)));

    if (new->data == NULL)
    {
        free(new);
        return NULL;
    }

    return new;
}

void vector_append(struct vector *vector, char value)
{
    if (vector->size >= vector->capacity)
    {
        vector->capacity *= 2;
        vector->data = realloc(vector->data, vector->capacity * sizeof(char));
    }

    vector->data[vector->size] = value;
    vector->size++;
}

void vector_destroy(struct vector *vector)
{
    free(vector->data);
    free(vector);
}
