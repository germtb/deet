#include "utils.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

void test_concat()
{
    assert(strcmp(concat("hello ", "you"), "hello you") == 0);
    assert(strcmp(concat("hello", ""), "hello") == 0);
    assert(strcmp(concat("", "hello"), "hello") == 0);
}

int main()
{
    test_concat();
}
