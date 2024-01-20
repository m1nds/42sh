#include "hash_map.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hash_map *hash_map_init(size_t size)
{
    struct hash_map *h = malloc(sizeof(struct hash_map));
    if (!h)
    {
        return NULL;
    }
    h->size = size;
    h->data = calloc(size, sizeof(struct pair_list *));
    if (!(h->data))
    {
        return NULL;
    }
    return h;
}

bool hash_map_insert(struct hash_map *hash_map, const char *key, char *value,
                     bool *updated)
{
    if (!hash_map)
    {
        return false;
    }
    if (hash_map->size == 0)
    {
        return false;
    }
    size_t h = hash(key) % hash_map->size;
    if (hash_map->data[h])
    {
        struct pair_list *x = hash_map->data[h];
        while (x)
        {
            if (strcmp(x->key, key) == 0)
            {
                x->value = value;
                *updated = true;
                return true;
            }
            x = x->next;
        }
    }
    if (hash_map->data[h] == NULL)
    {
        struct pair_list *elt = malloc(sizeof(struct pair_list));
        if (!elt)
        {
            return false;
        }
        elt->next = NULL;
        elt->key = key;
        elt->value = value;
        hash_map->data[h] = elt;
    }
    else
    {
        struct pair_list *elt = malloc(sizeof(struct pair_list));
        elt->next = hash_map->data[h];
        elt->key = key;
        elt->value = value;
        hash_map->data[h] = elt;
    }
    *updated = false;
    return true;
}

void hash_map_free(struct hash_map *hash_map)
{
    if (!hash_map)
    {
        return;
    }
    for (size_t k = 0; k < hash_map->size; k++)
    {
        struct pair_list *pl = hash_map->data[k];
        while (pl)
        {
            struct pair_list *temp = pl;
            pl = pl->next;
            free(temp);
        }
    }
    free(hash_map->data);
    free(hash_map);
}

void hash_map_dump(struct hash_map *hash_map)
{
    if (!hash_map)
    {
        return;
    }
    for (size_t k = 0; k < hash_map->size; k++)
    {
        if (hash_map->data[k])
        {
            struct pair_list *pl = hash_map->data[k];
            while (pl)
            {
                printf("%s: %s", pl->key, pl->value);
                pl = pl->next;
                if (pl)
                {
                    printf(", ");
                }
                else
                {
                    printf("\n");
                }
            }
        }
    }
}

const char *hash_map_get(const struct hash_map *hash_map, const char *key)
{
    if (!hash_map || !key)
    {
        return NULL;
    }
    if (hash_map->size == 0)
    {
        return NULL;
    }
    size_t h = hash(key) % hash_map->size;
    if (hash_map->data[h])
    {
        struct pair_list *lp = hash_map->data[h];
        while (lp)
        {
            if (strcmp(lp->key, key) == 0)
            {
                return lp->value;
            }
            lp = lp->next;
        }
    }
    return NULL;
}

bool hash_map_remove(struct hash_map *hash_map, const char *key)
{
    if (!hash_map)
    {
        return false;
    }
    if (hash_map->size == 0)
    {
        return false;
    }
    size_t h = hash(key) % hash_map->size;
    struct pair_list *lp = hash_map->data[h];
    struct pair_list *prev = hash_map->data[h];

    if (!lp)
    {
        return false;
    }

    if (strcmp(lp->key, key) == 0)
    {
        hash_map->data[h] = lp->next;
        free(lp);
        return true;
    }
    lp = lp->next;
    while (lp)
    {
        if (strcmp(lp->key, key) == 0)
        {
            prev->next = lp->next;
            free(lp);
            return true;
        }
        prev = lp;
        lp = lp->next;
    }
    return false;
}
