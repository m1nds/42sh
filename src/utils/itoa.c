#define _POSIX_C_SOURCE 200809L
#include "utils/itoa.h"

#include <stddef.h>
#include <string.h>

#include "utils/vector.h"

char *itoa_base(unsigned int value)
{
    if (value == 0)
    {
        return strdup("0");
    }
    struct vector *vec = vector_create(100);
    unsigned int save = value;
    while (save)
    {
        vector_append(vec, (save % 10) + '0');
        save /= 10;
    }

    vector_append(vec, '\0');

    size_t start = 0;
    size_t end = strlen(vec->data) - 1;

    while (start < end && end != 0)
    {
        char tmp = vec->data[start];
        vec->data[start] = vec->data[end];
        vec->data[end] = tmp;

        start++;
        end--;
    }
    char *s = strdup(vec->data);
    vector_destroy(vec);
    return s;
}
