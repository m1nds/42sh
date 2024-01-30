#include "utils/hash_map.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"

struct hash_map *hash_map_init(size_t size, enum hash_map_type type)
{
    struct hash_map *h = malloc(sizeof(struct hash_map));
    if (!h)
    {
        return NULL;
    }
    h->size = size;
    h->type = type;
    h->data = calloc(size, sizeof(struct pair_list *));
    if (!(h->data))
    {
        return NULL;
    }
    return h;
}

bool hash_map_insert(struct hash_map *hash_map, char *key, void *value)
{
    if (!hash_map || hash_map->size == 0)
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
                free(x->value);
                free(key);
                // memcpy(x->value, value, size);
                x->value = value;
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
            free(temp->key);
            if (hash_map->type == HASH_MAP_AST)
            {
                ast_free(temp->value);
            }
            else
            {
                free(temp->value);
            }
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
                printf("%s: %p", pl->key, pl->value);
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

void *hash_map_get(const struct hash_map *hash_map, char *key)
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
                if (hash_map->type == HASH_MAP_AST)
                {
                    struct ast *ret = lp->value;
                    return ret;
                }
                else
                {
                    char *ret = lp->value;
                    return ret;
                }
            }
            lp = lp->next;
        }
    }
    return NULL;
}

bool hash_map_remove(struct hash_map *hash_map, char *key)
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
        if (hash_map->type == HASH_MAP_AST)
        {
            ast_free(lp->value);
        }
        else
        {
            free(lp->value);
        }
        free(lp->key);
        free(lp);
        return true;
    }
    lp = lp->next;
    while (lp)
    {
        if (strcmp(lp->key, key) == 0)
        {
            prev->next = lp->next;
            if (hash_map->type == HASH_MAP_AST)
            {
                ast_free(lp->value);
            }
            else
            {
                free(lp->value);
            }
            free(lp->key);
            free(lp);
            return true;
        }
        prev = lp;
        lp = lp->next;
    }
    return false;
}

struct hash_map *hash_map_copy(struct hash_map *hash_map)
{
    if (hash_map == NULL)
    {
        return NULL;
    }
    struct hash_map *copy = hash_map_init(hash_map->size, hash_map->type);
    for (size_t k = 0; k < hash_map->size; k++)
    {
        if (hash_map->data[k] != NULL)
        {
            struct pair_list *pl = hash_map->data[k];
            struct pair_list *pl_copy = malloc(sizeof(struct pair_list));
            copy->data[k] = pl_copy;
            pl_copy->key = strdup(pl->key);
            if (hash_map->type == HASH_MAP_AST)
            {
                pl_copy->value = ast_deep_copy(pl->value);
            }
            else
            {
                pl_copy->value = strdup(pl->value);
            }
            pl_copy->next = NULL;
            while (pl->next != NULL)
            {
                pl = pl->next;
                pl_copy->next = malloc(sizeof(struct pair_list));
                pl_copy = pl_copy->next;
                pl_copy->key = strdup(pl->key);
                if (hash_map->type == HASH_MAP_AST)
                {
                    pl_copy->value = ast_deep_copy(pl->value);
                }
                else
                {
                    pl_copy->value = strdup(pl->value);
                }
                pl_copy->next = NULL;
            }
        }
        else
        {
            copy->data[k] = NULL;
        }
    }
    return copy;
}
