#include "vector.h"

#include <stdlib.h>
#include <string.h>

struct vector *vector_create(size_t capacity)
{
    struct vector *new = calloc(1, sizeof(struct vector));
    if (new == NULL)
    {
        return NULL;
    }

    new->capacity = capacity;
    new->data = calloc(capacity, sizeof(char));

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

char vector_pop(struct vector *vector)
{
    if (vector->size == 0)
    {
        return '\0';
    }

    char out = vector->data[vector->size];
    vector->size--;

    return out;
}

void vector_append_string(struct vector *vector, char *str)
{
    if (str == NULL)
    {
        return;
    }
    size_t i = 0;
    while (str[i] != '\0')
    {
        vector_append(vector, str[i]);
        i++;
    }
}

void vector_destroy(struct vector *vector)
{
    free(vector->data);
    free(vector);
}
