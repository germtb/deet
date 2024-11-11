#ifndef HASHMAP_H_
#define HASHMAP_H_

#include "../zone/zone.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct Hashmap
{
    uint32_t capacity;
    uint32_t count;
    struct HashmapEntry **entries;
} Hashmap;

typedef struct HashmapEntry
{
    char *key;
    void *value;
    struct HashmapEntry *next;
} HashmapEntry;

// Hashing functions
uint64_t hash(const char *data);
uint64_t hashmod(const char *data, uint64_t mod);

// Hashmap
Hashmap *make_hashmap(Zone *zone, uint32_t capacity);
void *hashmap_get(Hashmap *map, char *key);
bool hashmap_has(Hashmap *map, char *key);
void hashmap_set(Zone *zone, Hashmap *map, char *key, void *value);
void hashmap_resize(Zone *zone, Hashmap *map, uint32_t new_size);
HashmapEntry *hashmap_get_entry(Hashmap *map, char *key);

// Iterator
HashmapEntry *hashmap_iterator(Zone *zone, Hashmap *map);

// Debug
void print_hashmap(Hashmap *map, void (*print)(char *key, void *value));
void print_iterator(HashmapEntry *entry);

#endif // HASHMAP_H_
