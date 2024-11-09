#include "./zone.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_create_zone()
{
    Zone zone = make_zone(1);
    assert(zone.capacity == 1);
    assert(zone.pointer == 0);
}

void test_allocate()
{
    Zone zone = make_zone(1024);
    char *string = zalloc(&zone, 5);
    string[0] = 'h';
    string[1] = 'e';
    string[2] = 'l';
    string[3] = 'l';
    string[4] = 'o';
    string[5] = '\0';
    assert(strcmp(string, "hello") == 0);
    free_zone(&zone);
    assert(zone.next == NULL);
}

void test_create_linked_zone()
{
    Zone zone = make_zone(1);
    char *string = zalloc(&zone, 5);
    assert(zone.next != NULL);
    assert(string != NULL);
    string[0] = 'h';
    string[1] = 'e';
    string[2] = 'l';
    string[3] = 'l';
    string[4] = 'o';
    string[5] = '\0';
    assert(strcmp(string, "hello") == 0);
    free_zone(&zone);
    assert(zone.next == NULL);
}

int main()
{
    test_create_zone();
    test_allocate();
    test_create_linked_zone();
}
