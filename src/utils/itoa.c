#include <stddef.h>

#include "my_itoa.h"

char *my_itoa(int value, char *s)
{
    size_t i = 0;
    if (value == 0)
    {
        s[i++] = '0';
        s[i] = '\0';

        return s;
    }

    int neg = (value < 0) ? 1 : 0;

    if (neg)
    {
        value = -value;
        s[i] = '-';
        i++;
    }

    int save = value;
    while (save)
    {
        s[i] = (save % 10) + '0';

        save /= 10;
        i++;
    }

    s[i] = '\0';

    size_t start = neg;
    size_t end = i - 1;

    while (start < end)
    {
        char tmp = s[start];
        s[start] = s[end];
        s[end] = tmp;

        start++;
        end--;
    }

    return s;
}
