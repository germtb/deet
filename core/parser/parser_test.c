#include "./parser.h"
#include "../lexer/lexer.h"
#include "../debug/debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>

typedef struct TraverseAcc
{
    int index;
    enum NodeType *expectations;
} TraverseAcc;

enum NodeType get_expected(TraverseAcc *acc)
{
    return acc->expectations[acc->index];
}

bool traverse_expect(struct Node *node, TraverseAcc *acc)
{
    enum NodeType expected = get_expected(acc);
    acc->index++;

    if (node == NULL)
    {
        return true;
    }
    else if (node->type != expected)
    {
        printf("\033[31;1mError:\033[0m expected %s but got \033[31;1m%s\033[0m\n", get_node_name(expected), get_node_name(node->type));
        return false;
    }

    NodeList *list = node->head;

    while (list != NULL)
    {
        if (!traverse_expect(list->value, acc))
        {
            // Stop checking after the first error
            return false;
        }
        list = list->next;
    }

    return true;
}

int counter = 0;

void parse_and_expect_types(char *src, enum NodeType *expectations)
{
    int test = ++counter;
    TraverseAcc acc = {0, expectations};
    struct Node *node = parse(src);
    if (traverse_expect(node, &acc) == false)
    {
        printf("Faile test %d, with src: %s", test, src);
        printf("\n\nTokens:\n\n");
        print_tokens(src);
        printf("\n\nAST:\n\n");
        print_ast(src);
        assert(false);
    }
}

int main()
{
    {
        enum NodeType expected[] = {Program, Statement, Number, End};
        parse_and_expect_types("2;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Number, End};
        parse_and_expect_types("2;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, And, True, True, End};
        parse_and_expect_types("true && true;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Or, True, True, End};
        parse_and_expect_types("true || true;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Or, True, Or, True, True, End};
        parse_and_expect_types("true || true || true;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Xor, True, True, End};
        parse_and_expect_types("true ^ true;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, BitAnd, Number, Number, End};
        parse_and_expect_types("0 & 1;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, BitOr, Number, Number, End};
        parse_and_expect_types("0 | 1;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Identifier, End};
        parse_and_expect_types("foo;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, PreIncrement, Identifier, End};
        parse_and_expect_types("++foo;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, PostIncrement, Identifier, End};
        parse_and_expect_types("foo++;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Not, True, End};
        parse_and_expect_types("!true;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Negative, Number, End};
        parse_and_expect_types("-1;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Number, End};
        parse_and_expect_types("+1;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Number, End};
        parse_and_expect_types("const foo = +1;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Identifier, End};
        parse_and_expect_types("foo;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Call, Identifier, CallArgs, End};
        parse_and_expect_types("foo();", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Access, Identifier, Identifier, End};
        parse_and_expect_types("foo.bar;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Call, Access, Identifier, Identifier, CallArgs, End};
        parse_and_expect_types("foo.bar();", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Call, Access, Call, Identifier, CallArgs, Identifier, CallArgs, End};
        parse_and_expect_types("foo().bar();", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Access, Call, Identifier, CallArgs, Identifier, End};
        parse_and_expect_types("foo().bar;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Number, End};
        parse_and_expect_types("const foo = 2;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, StringLiteral, End};
        parse_and_expect_types("const foo = \"Hello\";", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, True, End};
        parse_and_expect_types("var foo = true;", expected);
    }
    {
        enum NodeType expected[] = {Program, TypeDeclaration, TypeIdentifier, TypeIdentifier, End};
        parse_and_expect_types("type foo = u8;", expected);
    }
    {
        enum NodeType expected[] = {Program, TypeDeclaration, TypeIdentifier, TypeIdentifier, End};
        parse_and_expect_types("type foo = u8;", expected);
    }
    {
        enum NodeType expected[] = {Program, TypeDeclaration, TypeIdentifier, Array, TypeIdentifier, End};
        parse_and_expect_types("type foo = u8[];", expected);
    }
    {
        enum NodeType expected[] = {Program, TypeDeclaration, TypeIdentifier, Struct, StructProperty, Identifier, TypeIdentifier, End};
        parse_and_expect_types("type foo = { bar: u8 };", expected);
    }
    {
        enum NodeType expected[] = {Program, TypeDeclaration, TypeIdentifier, Struct, StructProperty, Identifier, TypeIdentifier, StructProperty, Identifier, TypeIdentifier, End};
        parse_and_expect_types("type foo = { bar: u8, goo: u32 };", expected);
    }
    {
        enum NodeType expected[] = {Program, TypeDeclaration, TypeIdentifier, Struct, StructProperty, Identifier, Array, TypeIdentifier, End};
        parse_and_expect_types("type foo = { bar: u8[] };", expected);
    }
    {
        enum NodeType expected[] = {Program, TypeDeclaration, TypeIdentifier, Struct, StructProperty, Identifier, Struct, End};
        parse_and_expect_types("type foo = { bar: {} };", expected);
    }
    {
        enum NodeType expected[] = {Program, TypeDeclaration, TypeIdentifier, Struct, StructProperty, Identifier, Struct, StructProperty, Identifier, TypeIdentifier, End};
        parse_and_expect_types("type foo = { bar: { bar: u8 } };", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Struct, StructProperty, Identifier, Int32, End};
        parse_and_expect_types("const foo = { bar: 1i32 };", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Struct, StructProperty, Identifier, Uint16, End};
        parse_and_expect_types("const foo = { bar: 3u16 };", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Struct, StructProperty, Identifier, Decimal, StructProperty, Identifier, Decimal, End};
        parse_and_expect_types("const foo = { bar: 1.0, goo: 2.0 };", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Struct, StructProperty, Identifier, Struct, End};
        parse_and_expect_types("const foo = { bar: {} };", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Struct, StructProperty, Identifier, Struct, StructProperty, Identifier, True, End};
        parse_and_expect_types("const foo = { bar: { bar: true } };", expected);
    }
    {
        enum NodeType expected[] = {Program, Reassignment, Identifier, Number, End};
        parse_and_expect_types("foo = 123;", expected);
    }
    {
        enum NodeType expected[] = {Program, StructDeclaration, TypeIdentifier, End};
        parse_and_expect_types("struct Position {};", expected);
    }
    {
        enum NodeType expected[] = {Program, StructDeclaration, TypeIdentifier, StructProperty, Identifier, TypeIdentifier, End};
        parse_and_expect_types("struct S { x: u8 };", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, TypeIdentifier, Number, End};
        parse_and_expect_types("const foo: u8 = 10;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Function, FunctionArgs, Number, End};
        parse_and_expect_types("const foo = fn () => 10;", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, Number, Declaration, Identifier, Function, FunctionArgs, Number, End};
        parse_and_expect_types("10; const foo = fn () => 10;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Function, FunctionArgs, Identifier, End};
        parse_and_expect_types("const foo = fn () => x;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Function, FunctionArgs, FunctionArg, Identifier, Identifier, End};
        parse_and_expect_types("const foo = fn (x) => x;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Function, FunctionArgs, FunctionArg, Identifier, TypeIdentifier, Identifier, End};
        parse_and_expect_types("const foo = fn (x: u8) => x;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Function, FunctionArgs, TypeIdentifier, Identifier, End};
        parse_and_expect_types("const foo = fn (): u32 => x;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Add, Number, Number, End};
        parse_and_expect_types("const foo = 1 + 2;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Add, Mul, Number, Number, Number, End};
        parse_and_expect_types("const foo = 1 * 2 + 3;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Add, Number, Mul, Number, Number, End};
        parse_and_expect_types("const foo = 1 + 2 * 3;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Eq, Not, True, False, End};
        parse_and_expect_types("const foo = !true == false;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Ge, Add, Number, Number, Number, End};
        parse_and_expect_types("const foo = 1 + 2 >= 0;", expected);
    }
    {
        enum NodeType expected[] = {Program, If, True, Block, End};
        parse_and_expect_types("if (true) {} ", expected);
    }
    {
        enum NodeType expected[] = {Program, If, True, Block, Statement, Number, End};
        parse_and_expect_types("if (true) { 1; }", expected);
    }
    {
        enum NodeType expected[] = {Program, If, True, Block, Statement, Number, Else, Block, End};
        parse_and_expect_types("if (true) { 1; } else {}", expected);
    }
    {
        enum NodeType expected[] = {Program, If, True, Block, Statement, Number, Else, Block, Statement, Number, End};
        parse_and_expect_types("if (true) { 1; } else { 1; }", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Call, Identifier, CallArgs, End};
        parse_and_expect_types("const foo = bar();", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Call, Identifier, CallArgs, CallArg, Number, End};
        parse_and_expect_types("const foo = bar(12);", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Call, Identifier, CallArgs, CallArg, Identifier, Number, End};
        parse_and_expect_types("const foo = bar(x: 12);", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Access, Identifier, Identifier, End};
        parse_and_expect_types("const foo = foo.bar;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Access, Access, Identifier, Identifier, Identifier, End};
        parse_and_expect_types("const foo = foo.bar.test;", expected);
    }
    {
        enum NodeType expected[] = {Program, Declaration, Identifier, Function, FunctionArgs, TypeIdentifier, Block, Return, Number, End};
        parse_and_expect_types("const foo = fn (): u8 => {\n\treturn 10;\n};", expected);
    }
    {
        enum NodeType expected[] = {Program, Comment, Statement, Number, End};
        parse_and_expect_types("// Comment\n123;\n", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, StringInterpolation, Identifier, StringInterpolation, End};
        parse_and_expect_types("\"{foo}\";", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, StringInterpolation, Identifier, Identifier, StringInterpolation, End};
        parse_and_expect_types("\"{foo}{bar}\";", expected);
    }
    {
        enum NodeType expected[] = {Program, Statement, StringInterpolation, Identifier, StringInterpolation, Identifier, StringInterpolation, End};
        parse_and_expect_types("\"{foo} {bar}\";", expected);
    }
    {
        enum NodeType expected[] = {Program, FunctionDeclaration, Identifier, FunctionArgs, TypeIdentifier, Block, Return, Number, End};
        parse_and_expect_types(
            "fn main(): u8 {\n"
            "  return 10;\n"
            "}",
            expected);
    }
}
