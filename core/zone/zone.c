#include "zone.h"

#include <stdio.h>
#include <stdlib.h>

static void init_zone(Zone *zone, size_t capacity)
{
    if (capacity <= 0)
    {
        printf("Invalid capacity, it needs to be greater than 0\n");
        return;
    }
    zone->capacity = capacity;
    zone->pointer = 0;
    zone->memory = malloc(capacity * sizeof(uint8_t));
}

Zone make_zone(size_t capacity)
{
    Zone zone = {};
    init_zone(&zone, capacity);
    return zone;
}

void *zalloc(Zone *zone, size_t size)
{
    Zone *page = zone;

    if (page == NULL)
    {
        return NULL;
    }

    if (page->capacity == 0)
    {
        // log an error
        printf("Trying to allocate in a zone that has been freed\n");
        return NULL;
    }

    Zone *prev = NULL;

    while (page != NULL && (size > (page->capacity - page->pointer)))
    {
        prev = page;
        page = page->next;
    }

    if (page == NULL)
    {
        Zone *next_zone = malloc(sizeof(Zone));

        if (next_zone == NULL)
        {
            return NULL;
        }

        init_zone(next_zone, prev->capacity * 2);
        prev->next = next_zone;

        return zalloc(next_zone, size);
    }

    uint8_t *address = (page->memory + page->pointer);
    page->pointer += size;
    return address;
}

void free_zone(Zone *zone)
{
    if (zone->next != NULL)
    {
        free_zone(zone->next);
    }
    free(zone->memory);
    free(zone->next);
    zone->next = NULL;
    zone->capacity = 0;
    zone->pointer = 0;
}
