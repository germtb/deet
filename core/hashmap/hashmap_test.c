#include "./hashmap.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

void test_hash(Zone *z)
{
    uint64_t i = hash("foo");
    assert(i == 15902901984413996407ull);
}

void test_hashmod(Zone *z)
{
    uint64_t i = hashmod("foo", 10);
    assert(i == 7);
}

void test_hashmap_init(Zone *z)
{
    Hashmap *map = make_hashmap(z, 10);
    assert(map->capacity == 10);
    assert(map->entries != NULL);
}

void test_hashmap_get(Zone *z)
{
    Hashmap *map = make_hashmap(z, 10);
    void *entry = hashmap_get(map, "foo");
    assert(entry == NULL);
}

void test_hashmap_get_entry(Zone *z)
{
    Hashmap *map = make_hashmap(z, 10);
    HashmapEntry *entry = hashmap_get_entry(map, "foo");
    assert(entry == NULL);
}

void test_hashmap_set(Zone *z)
{
    Hashmap *map = make_hashmap(z, 10);
    hashmap_set(z, map, "foo", "bar");
    assert(map->count == 1);
}

void test_hashmap_set_with_same_key(Zone *z)
{
    Hashmap *map = make_hashmap(z, 10);
    hashmap_set(z, map, "foo", "bar");
    hashmap_set(z, map, "foo", "baz");
    assert(map->count == 1);
}

void test_hashmap_set_with_collision(Zone *z)
{
    Hashmap *map = make_hashmap(z, 1);
    hashmap_set(z, map, "foo", "bar");
    hashmap_set(z, map, "bar", "baz");
    assert(map->count == 2);
}

void test_hashmap_set_then_get(Zone *z)
{
    Hashmap *map = make_hashmap(z, 10);
    hashmap_set(z, map, "foo", "bar");
    char *entry = hashmap_get(map, "foo");
    assert(entry != NULL);
    assert(strcmp(entry, "bar") == 0);
}

void test_hashmap_set_then_get_entry(Zone *z)
{
    Hashmap *map = make_hashmap(z, 10);
    hashmap_set(z, map, "foo", "bar");
    HashmapEntry *entry = hashmap_get_entry(map, "foo");
    assert(entry != NULL);
    assert(strcmp(entry->value, "bar") == 0);
    assert(strcmp(entry->key, "foo") == 0);
    assert(entry->next == NULL);
}

void test_hashmap_many_sets_many_gets(Zone *z)
{

    int sizes[] = {1, 10, 100, 1000, 10000};

    for (int size_index = 0; size_index < 5; size_index++)
    {
        int capacity = sizes[size_index];

        Hashmap *map = make_hashmap(z, 100);

        char *keys[] = {
            "1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
            "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
            "21", "22", "23", "24", "25", "26", "27", "28", "29", "30",
            "31", "32", "33", "34", "35", "36", "37", "38", "39", "40",
            "41", "42", "43", "44", "45", "46", "47", "48", "49", "50",
            "51", "52", "53", "54", "55", "56", "57", "58", "59", "60",
            "61", "62", "63", "64", "65", "66", "67", "68", "69", "70",
            "71", "72", "73", "74", "75", "76", "77", "78", "79", "80",
            "81", "82", "83", "84", "85", "86", "87", "88", "89", "90",
            "91", "92", "93", "94", "95", "96", "97", "98", "99", "100"};

        for (int i = 0; i < 100; i++)
        {
            hashmap_set(z, map, keys[i], keys[i]);
        }

        assert(map->count == 100);

        for (int i = 0; i < 100; i++)
        {
            char *entry = hashmap_get(map, keys[i]);
            assert(entry != NULL);
            assert(strcmp(entry, keys[i]) == 0);
        }
    }
}

void test_hashmap_iterator(Zone *z)
{
    Hashmap *map = make_hashmap(z, 5);

    char *keys[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};

    for (int i = 0; i < 10; i++)
    {
        hashmap_set(z, map, keys[i], keys[i]);
    }

    HashmapEntry *iterator = hashmap_iterator(z, map);

    Hashmap *new_map = make_hashmap(z, 5);

    while (iterator != NULL)
    {
        hashmap_set(z, new_map, iterator->value, iterator->value);
        iterator = iterator->next;
    }

    assert(map->count == new_map->count);

    for (int i = 0; i < 10; i++)
    {
        assert(hashmap_has(new_map, keys[i]));
    }
}

void test_hashmap_resize(Zone *z)
{
    Hashmap *map = make_hashmap(z, 1);
    hashmap_set(z, map, "foo", "bar");
    hashmap_set(z, map, "bar", "baz");

    hashmap_resize(z, map, 10);

    assert(map->capacity == 10);
    assert(map->count == 2);

    hashmap_resize(z, map, 1);

    assert(map->capacity == 1);
    assert(map->count == 2);
}

int main()
{
    Zone z = make_zone(1024);

    test_hash(&z);
    test_hashmod(&z);
    test_hashmap_init(&z);
    test_hashmap_get(&z);
    test_hashmap_get_entry(&z);
    test_hashmap_set(&z);
    test_hashmap_set_with_same_key(&z);
    test_hashmap_set_with_collision(&z);
    test_hashmap_set_then_get(&z);
    test_hashmap_set_then_get_entry(&z);
    test_hashmap_many_sets_many_gets(&z);
    test_hashmap_iterator(&z);
    test_hashmap_resize(&z);

    free_zone(&z);

    return 0;
}
