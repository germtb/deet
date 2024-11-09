#include "array.h"
#include "../string/string.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

Array *array(Zone *z, uint32_t capacity)
{
    if (capacity <= 0)
    {
        return NULL;
    }

    Array *a = zalloc(z, sizeof(Array));
    a->capacity = capacity;
    a->data = zalloc(z, sizeof(void *) * capacity);
    a->size = 0;
    return a;
}

Array *array_from(Zone *zone, ...)
{
    Array *a = array(zone, 10);

    va_list args;
    va_start(args, zone);

    void *element;

    while ((element = va_arg(args, String *)) != NULL)
    {
        array_push(zone, a, element);
    }

    va_end(args);

    return a;
}

void *array_at(Array *a, uint32_t index)
{
    if (index >= a->size)
    {
        return NULL;
    }

    return a->data[index];
}

void array_push(Zone *zone, Array *a, void *data)
{
    if (a->size == a->capacity)
    {
        size_t new_capacity = a->capacity * 2;
        void *new_data = zalloc(zone, sizeof(void *) * new_capacity);
        memcpy(new_data, a->data, sizeof(void *) * a->size);
        a->data = new_data;
        a->capacity = new_capacity;
    }

    a->data[a->size++] = data;
}

void *array_pop(Array *a)
{
    if (a->size == 0)
    {
        return NULL;
    }

    return a->data[--a->size];
}

String *array_join(Zone *zone, Array *a, char c)
{
    int size = 0;

    for (uint32_t i = 0; i < a->size; i++)
    {
        String *s = array_at(a, i);

        assert(s != NULL);
        size += s->length;

        if (i < a->size - 1)
        {
            size += 1;
        }
    }

    String *result = str_len(zone, size);

    int acc = 0;

    for (uint32_t i = 0; i < a->size; i++)
    {
        String *s = array_at(a, i);
        memcpy(result->data + acc, s->data, s->length);
        acc += s->length;

        if (i < a->size - 1)
        {
            memcpy(result->data + acc, &c, 1);
            acc += 1;
        }
    }

    return result;
}

String *array_empty_join(Zone *zone, Array *a)
{
    int size = 0;

    for (uint32_t i = 0; i < a->size; i++)
    {
        String *s = array_at(a, i);
        assert(s != NULL);
        size += s->length;
    }

    String *result = str_len(zone, size);

    int acc = 0;
    for (uint32_t i = 0; i < a->size; i++)
    {
        String *s = array_at(a, i);
        memcpy(result->data + acc, s->data, s->length);
        acc += s->length;
    }

    return result;
}
