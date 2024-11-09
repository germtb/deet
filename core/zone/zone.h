#ifndef ZONE_H_
#define ZONE_H_

#include <stdlib.h>

typedef struct Zone
{
    size_t capacity;
    size_t pointer;
    uint8_t *memory;
    struct Zone *next;
} Zone;

Zone make_zone(size_t capacity);
void *zalloc(Zone *zone, size_t capacity);
void free_zone(Zone *zone);

#endif // ZONE_H_
