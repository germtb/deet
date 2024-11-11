#include "./hashmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

uint64_t hash(const char *data)
{

    uint64_t hash = 14695981039346656037ull;
    char c = *data;

    while (c != '\0')
    {
        hash ^= c;
        hash *= 1099511628211ull;
        c = *++data;
    }

    return hash;
}

uint64_t hashmod(const char *data, uint64_t mod)
{
    return hash(data) % mod;
}

Hashmap *make_hashmap(Zone *zone, uint32_t capacity)
{
    Hashmap *map = zalloc(zone, sizeof(Hashmap));
    map->capacity = capacity;
    map->count = 0;
    map->entries = zalloc(zone, sizeof(HashmapEntry *) * capacity);

    for (uint32_t i = 0; i < capacity; i++)
    {
        map->entries[i] = NULL;
    }
    return map;
}

void hashmap_set(Zone *zone, Hashmap *map, char *key, void *value)
{
    uint64_t index = hashmod(key, map->capacity);

    HashmapEntry *prev = NULL;
    HashmapEntry *entry = map->entries[index];

    while (entry != NULL)
    {
        if (strcmp(entry->key, key) == 0)
        {
            entry->value = value;
            return;
        }

        prev = entry;
        entry = entry->next;
    }

    if (prev == NULL)
    {
        map->count++;
        map->entries[index] = zalloc(zone, sizeof(HashmapEntry));
        map->entries[index]->key = key;
        map->entries[index]->value = value;
    }
    else
    {
        map->count++;
        prev->next = zalloc(zone, sizeof(HashmapEntry));
        prev->next->key = key;
        prev->next->value = value;
    }
}

HashmapEntry *hashmap_get_entry(Hashmap *map, char *key)
{
    uint64_t index = hashmod(key, map->capacity);
    HashmapEntry *entry = map->entries[index];

    while (entry != NULL)
    {
        if (strcmp(entry->key, key) == 0)
        {
            return entry;
        }

        entry = entry->next;
    }

    return NULL;
}

bool hashmap_has(Hashmap *map, char *key)
{
    HashmapEntry *entry = hashmap_get_entry(map, key);

    if (entry == NULL)
    {
        return false;
    }

    return true;
}

void *hashmap_get(Hashmap *map, char *key)
{
    HashmapEntry *entry = hashmap_get_entry(map, key);

    if (entry == NULL)
    {
        return NULL;
    }

    return entry->value;
}

void hashmap_resize(Zone *zone, Hashmap *map, uint32_t new_size)
{
    Hashmap *new_map = make_hashmap(zone, new_size);
    HashmapEntry *iterator = hashmap_iterator(zone, map);
    HashmapEntry *entry = iterator;

    while (entry != NULL)
    {
        hashmap_set(zone, new_map, entry->key, entry->value);
        entry = entry->next;
    }

    *map = *new_map;
}

void hashmap_iterator_free(HashmapEntry *entry)
{
    HashmapEntry *current = entry;

    while (current != NULL)
    {
        // Create a copy of the next pointer before freeing the current entry
        HashmapEntry *next = entry->next;
        // Free the current entry
        free(current);
        // Move to the next entry
        current = next;
    }
}

HashmapEntry *hashmap_iterator(Zone *zone, Hashmap *map)
{
    HashmapEntry *head = NULL;
    head = NULL;
    HashmapEntry *tail = NULL;

    for (uint32_t i = 0; i < map->capacity; i++)
    {
        HashmapEntry *entry = map->entries[i];

        while (entry != NULL)
        {
            HashmapEntry *copied_entry = zalloc(zone, sizeof(HashmapEntry));
            copied_entry->key = entry->key;
            copied_entry->value = entry->value;
            copied_entry->next = NULL;

            if (head == NULL)
            {
                head = copied_entry;
                tail = copied_entry;
            }
            else
            {
                tail->next = copied_entry;
                tail = copied_entry;
            }

            entry = entry->next;
        }
    }

    return head;
}

void print_hashmap(Hashmap *map, void (*print)(char *key, void *value))
{
    for (uint32_t i = 0; i < map->capacity; i++)
    {
        HashmapEntry *entry = map->entries[i];

        if (entry == NULL)
        {
            print(NULL, NULL);
        }
        else
        {
            while (entry != NULL)
            {
                print(entry->key, entry->value);
                entry = entry->next;
            }
        }
    }
}

void print_iterator(HashmapEntry *entry)
{
    HashmapEntry *current = entry;
    while (current != NULL)
    {
        printf("{ key: %s, value: %s } -> ", current->key, (char *)current->value);
        current = current->next;
    }
}
