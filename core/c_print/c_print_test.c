#include "c_print.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool string_match_ignore_whitespace(const char *given, const char *expected)
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
            return false;
        }
    }

    return true;
}

void test(char *given, char *expected)
{
    char *s = c_print(given, false);
    if (!string_match_ignore_whitespace(s, expected))
    {
        printf("---\n");
        printf("Expected:\n%s\n", expected);
        printf("---\n");
        printf("Got:\n%s\n", given);
        printf("---\n");
        printf("Failed comparing char %c with %c\n", *s, *t);
        printf("---\n");
    }
}

int main()
{
    test("fn main() {}", "void main() {}");
    test("fn main(): u8 {}", "uint8_t main() {}");
}
