#ifndef ARRAY_H_
#define ARRAY_H_

#include <stdlib.h>
#include <stdbool.h>
#include "../zone/zone.h"
#include "../string/string.h"

typedef struct Array
{
    size_t capacity;
    uint32_t size;
    void **data;
} Array;

Array *array_from(Zone *zone, ...);
Array *array(Zone *z, uint32_t capacity);
void *array_at(Array *a, uint32_t index);
void array_push(Zone *zone, Array *a, void *data);
void *array_pop(Array *a);
String *array_join(Zone *zone, Array *a, char c);

#endif // ARRAY_H_
