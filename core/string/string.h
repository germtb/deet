#ifndef STRING_H_
#define STRING_H_

#include <stdlib.h>
#include <stdbool.h>
#include "../zone/zone.h"

typedef struct String
{
    uint32_t length;
    char *data;
} String;

String *str(Zone *z, const char *data);
String *str_len(Zone *z, uint32_t length);
bool str_equal(String *s, String *t);
bool str_equal_cstr(String *s, const char *t);
void str_fill(String *s, char c);
char *char_ptr_concat(char *str_1, char *str_2);
String *str_concat(Zone *z, ...);
String *str_template(Zone *zone, ...);

#endif /* STRING_H_ */
