#include "c_print.h"
#include "../debug/debug.h"
#include "../parser/parser.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

typedef struct TestAcc
{
    bool success;
} TestAcc;

void test(TestAcc *acc, char *given, char *expected)
{
    char *s = c_print(given, false);
    if (!string_match_ignore_whitespace(s, expected))
    {
        printf("---\n");
        printf("Expected:\n%s\n", expected);
        printf("---\n");
        printf("Got:\n%s\n", s);
        printf("---\n");
        acc->success = false;
    }
}

int main()
{
    TestAcc acc = (TestAcc){.success = true};

    test(&acc, "fn main() {}", "void main() {}");
    test(&acc, "fn main(): u8 {}", "uint8_t main() {}");
    test(&acc, "fn main(): u8 { return 10; }", "uint8_t main() { return 10; }");
    test(&acc, "fn main(): u8 { const i: u8 = 10; return i; }", "uint8_t main() { uint8_t i = 10; return i; }");
    test(&acc, "fn main() { const i = true; }", "void main() { bool i = true; }");
    test(&acc, "fn main() { const s = \"hello\"; }", "void main() { String s = str(\"hello\");}");
    test(&acc, "struct A {};", "typedef struct A {} A;");
    test(&acc, "struct A { name: String };", "typedef struct A { String name; } A;");
    test(&acc, "struct A { next: A };", "typedef struct A { struct A *next; } A;");
    test(&acc, "struct A {}; struct B { a: A };", "typedef struct A {} A; typedef struct B { A *a; } B;");
    test(&acc, "struct A {}; fn main(): A {}", "typedef struct A {} A; A *main() {}");
    test(&acc, "12u8;", "12;");
    test(&acc, "1 + 1;", "1 + 1;");
    test(&acc, "1 > 1;", "1 > 1;");
    test(&acc, "1 == 1;", "1 == 1;");
    test(&acc, "1 && 1;", "1 && 1;");
    test(&acc, "foo++;", "foo++;");
    test(&acc, "true;", "true;");
    test(&acc, "while(true) {}", "while(true) {}");
    test(&acc, "\"Hello {foo}\";", "str_template(zone, \"Hello %s\", foo);");
    test(&acc, "\"Hello {foo} {bar}\";", "str_template(zone, \"Hello %s %s\", foo, bar);");
    // test("struct A {}; const a: A = {};", "");
    // test("{ foo: 12u8 };", "");

    if (acc.success == true)
    {
        printf("All tests passed\n");
    }
}
