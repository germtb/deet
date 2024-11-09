#include "c_print.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void string_match_ignore_whitespace(const char *given, const char *expected)
{
    const char *s = given;
    const char *t = expected;

    while (true)
    {
        while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r')
        {
            s++;
        }

        while (*t == ' ' || *t == '\t' || *t == '\n' || *t == '\r')
        {
            t++;
        }

        if (*s == *t)
        {
            s++;
            t++;

            if (*s == '\0')
            {
                break;
            }
        }
        else
        {
            printf("---\n");
            printf("Expected:\n%s\n", expected);
            printf("---\n");
            printf("Got:\n%s\n", given);
            printf("---\n");
            printf("Failed comparing char %c with %c\n", *s, *t);
            printf("---\n");

            assert(0);
        }
    }
}

void test_1()
{
    char *s = c_print("fn main() {}\n", false);
    string_match_ignore_whitespace(s, "void main() {}");
}

int main()
{
    test_1();
}
