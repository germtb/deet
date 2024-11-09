#include "string.h"
#include "../array/array.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

String *str(Zone *z, const char *data)
{
    uint32_t length = strlen(data);
    String *s = zalloc(z, sizeof(String));
    s->data = zalloc(z, length + 1);
    // Is this correct?  Should we be copying the null terminator?
    memcpy(s->data, data, length);
    s->length = length;

    return s;
}

String *str_len(Zone *z, uint32_t length)
{
    String *s = zalloc(z, sizeof(String));
    s->data = zalloc(z, length + 1);
    s->length = length;
    // s->data[length + 1] = '\0';

    return s;
}

bool str_equal(String *s, String *t)
{
    if (&s == &t)
    {
        return true;
    }

    return s->length == t->length && memcmp(s->data, t->data, s->length) == 0;
}

bool str_equal_cstr(String *s, const char *t)
{
    return memcmp(s->data, t, s->length) == 0;
}

bool str_includes_cstr(String *s, const char *t)
{
    uint32_t length = strlen(t);

    for (uint32_t i = 0; i <= s->length - length; i++)
    {
        if (memcmp(s->data + i, t, length) == 0)
        {
            return true;
        }
    }

    return false;
}

bool str_includes(String *s, String *t)
{
    return str_includes_cstr(s, t->data);
}

char *char_ptr_concat(char *str_1, char *str_2)
{
    uint32_t len_1 = strlen(str_1);
    uint32_t len_2 = strlen(str_2);

    char *result = malloc((len_1 + len_2 + 1) * sizeof(char));

    for (uint32_t i = 0; i < len_1; i++)
    {
        result[i] = str_1[i];
    }
    for (uint32_t i = 0; i < len_2; i++)
    {
        result[len_1 + i] = str_2[i];
    }

    result[len_1 + len_2] = '\0';

    return result;
}

String *str_concat(Zone *zone, ...)
{
    va_list args;
    va_start(args, zone);

    String *s;
    String *result = str(zone, "");

    while ((s = va_arg(args, String *)) != NULL)
    {
        result = str(zone, char_ptr_concat(result->data, s->data));
    }

    va_end(args);

    return result;
}

void str_fill(String *s, char c)
{
    for (uint32_t i = 0; i < s->length; i++)
    {
        s->data[i] = c;
    }
}
