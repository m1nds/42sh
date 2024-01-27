#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdbool.h>
#include <stddef.h>

enum hash_map_type
{
    HASH_MAP_STRING,
    HASH_MAP_AST,
};

struct pair_list
{
    char *key;
    void *value;
    struct pair_list *next;
};

struct hash_map
{
    struct pair_list **data;
    size_t size;
    enum hash_map_type type;
};

/*
 * @def: Hash function
 * @args str: string to hash
 * @return: index in hash table
 */
size_t hash(char *str);

/*
 * @def: function to init the hash map
 * @args size: size of the hash map
 * @return: the initialised hash map
 */
struct hash_map *hash_map_init(size_t size, enum hash_map_type type);

/*
 * @def: function to insert element in hash map
 * @args hash_map: the hash map to insert in
 * @args key: the key of the inserted element
 * @args value: value of the inserted element
 * @args updated: true if key already in hash map and its value has changed,
 * else false
 * @return: return the bool updated
 */
bool hash_map_insert(struct hash_map *hash_map, char *key, void *value);
/*
 * @def: free the hash map
 * @args hash_map: the hash map to free
 */
void hash_map_free(struct hash_map *hash_map);

/*
 * @def: print the hash map
 * @args hash_map: the hash map to print
 */
void hash_map_dump(struct hash_map *hash_map);

/*
 * @def: get the value of an element in hash map
 * @args hash_map: hash map to search into
 * @args key: key of the element to search
 * @return: return the value of the searched element
 */
void *hash_map_get(const struct hash_map *hash_map, char *key);

/*
 * @def: remove an element from the hash map
 * @args hash_map: hash map
 * @args key: key of the element to remove
 * @return: true if the element has been removed else false
 */
bool hash_map_remove(struct hash_map *hash_map, char *key);

/*
 * @def: Returns a deep_copy of the hash_map passed in parameter
 * @args hash_map: The hash_map to copy
 * @return: A copy of the hash_map
 */
struct hash_map *hash_map_copy(struct hash_map *hash_map);

#endif /* ! HASH_MAP_H */
