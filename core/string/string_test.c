#include "./string.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void test_make_str(Zone *zone)
{
    String *s = str(zone, "Hello");
    assert(str_equal_cstr(s, "Hello"));
    assert(str_equal(s, s));
}

void test_str_length(Zone *zone)
{
    String *s = str(zone, "Hello");
    assert(s->length == 5);
}

void test_str_copy(Zone *zone)
{
    String *s = str(zone, "Hello");
    String *s2 = str(zone, s->data);
    assert(str_equal(s, s2));
    assert(&s != &s2);
    assert(&s->data != &s2->data);
    assert(memcmp(s->data, s2->data, s->length) == 0);
}

void test_str_concat(Zone *zone)
{
    String *s1 = str(zone, "Hello");
    String *s2 = str(zone, " World");
    String *s3 = str_concat(zone, s1, s2, NULL);
    assert(s1->length == 5);
    assert(s2->length == 6);
    assert(s3->length == 11);
    assert(strcmp(s3->data, "Hello World") == 0);
}

int main()
{
    Zone z = make_zone(1024);
    test_make_str(&z);
    test_str_length(&z);
    test_str_copy(&z);
    test_str_concat(&z);
    free_zone(&z);
}
