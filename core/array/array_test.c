#include "../zone/zone.h"
#include "array.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_array(Zone *z)
{
    Array *a = array(z, 10);
    assert(a->capacity == 10);
    assert(a->size == 0);
}

void test_array_from(Zone *z)
{
    Array *a = array_from(z, "hello", "world", NULL);
    assert(a->size == 2);
    assert(strcmp(array_at(a, 0), "hello") == 0);
    assert(strcmp(array_at(a, 1), "world") == 0);
    assert(array_at(a, 2) == NULL);
    assert(strcmp(array_pop(a), "world") == 0);
    assert(strcmp(array_pop(a), "hello") == 0);
}

void test_array_push(Zone *z)
{
    Array *a = array(z, 10);
    array_push(z, a, "hello");
    assert(a->capacity == 10);
    assert(a->size == 1);
}

void test_array_with_struct(Zone *z)
{
    Array *a = array(z, 10);
    String *s = str(z, "hello");

    array_push(z, a, s);
    assert(a->capacity == 10);
    assert(a->size == 1);

    String *s2 = array_at(a, 0);
    assert(strcmp((s2)->data, "hello") == 0);
    assert(s2->length == 5);

    String *s3 = array_pop(a);
    assert(strcmp(s3->data, "hello") == 0);
    assert(s3->length == 5);
}

void test_array_pop(Zone *z)
{
    Array *a = array(z, 10);
    array_push(z, a, "hello");
    char *s = array_pop(a);
    assert(a->capacity == 10);
    assert(a->size == 0);
    assert(strcmp(s, "hello") == 0);
}

void test_array_resize(Zone *z)
{
    Array *a = array(z, 1);
    array_push(z, a, "hello");
    assert(a->capacity == 1);
    array_push(z, a, "world");
    assert(a->capacity == 2);
    array_push(z, a, "!");
    assert(a->capacity == 4);
    assert(a->size == 3);

    assert(strcmp(array_at(a, 0), "hello") == 0);
    assert(strcmp(array_at(a, 1), "world") == 0);
    assert(strcmp(array_at(a, 2), "!") == 0);
    assert(array_at(a, 3) == NULL);

    assert(strcmp(array_pop(a), "!") == 0);
    assert(strcmp(array_pop(a), "world") == 0);
    assert(strcmp(array_pop(a), "hello") == 0);

    assert(a->size == 0);
    assert(a->capacity == 4);
}

void test_join(Zone *z)
{
    Array *a1 = array_from(z, str(z, "hello"), str(z, "world"), NULL);
    assert(strcmp(array_join(z, a1, ' ')->data, "hello world") == 0);

    Array *a2 = array_from(z, str(z, "hello"), str(z, "world"), NULL);
    assert(strcmp(array_join(z, a2, '!')->data, "hello!world") == 0);
}

int main()
{
    Zone z = make_zone(1024);
    test_array(&z);
    test_array_from(&z);
    test_array_push(&z);
    test_array_with_struct(&z);
    test_array_pop(&z);
    test_array_resize(&z);
    test_join(&z);
    free_zone(&z);
}
