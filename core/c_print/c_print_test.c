#include "c_print.h"
#include "../debug/debug.h"
#include "../parser/parser.h"

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
            printf("Failed comparing %s and %s\n", s, t);
            return false;
        }
    }

    return true;
}

int success = 1;

void test(char *given, char *expected)
{
    char *s = c_print(given, false);
    if (!string_match_ignore_whitespace(s, expected))
    {
        printf("---\n");
        printf("Expected:\n%s\n", expected);
        printf("---\n");
        printf("Got:\n%s\n", s);
        printf("---\n");
        success = success == 0;
    }
}

int main()
{
    test("fn main() {}", "void main() {}");
    test("fn main(): u8 {}", "uint8_t main() {}");
    test("fn main(): u8 { return 10; }", "uint8_t main() { return 10; }");
    test("fn main(): u8 { const i: u8 = 10; return i; }", "uint8_t main() { uint8_t i = 10; return i; }");
    test("fn main() { const i = true; }", "void main() { bool i = true; }");
    test("fn main() { const s = \"hello\"; }", "void main() { String s = str(\"hello\");}");
    test("struct A {};", "typedef struct A {} A;");
    test("struct A { name: String };", "typedef struct A { String name; } A;");
    test("struct A { next: A };", "typedef struct A { struct A *next; } A;");
    test("struct A {}; struct B { a: A };", "typedef struct A {} A; typedef struct B { A *a; } B;");
    test("struct A {}; fn main(): A {}", "typedef struct A {} A; A *main() {}");
    test("12u8;", "12;");
    test("1 + 1;", "1 + 1;");
    test("1 > 1;", "1 > 1;");
    test("1 == 1;", "1 == 1;");
    test("1 && 1;", "1 && 1;");
    test("foo++;", "foo++;");
    test("true;", "true;");
    // test("struct A {}; const a: A = {};", "");
    // test("{ foo: 12u8 };", "");

    if (success == 1)
    {
        printf("All tests passed\n");
    }
}
