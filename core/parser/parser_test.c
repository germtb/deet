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
            // NStop checking after the first error
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
        enum NodeType expected[] = {NProgram, NStatement, NNumber, NEnd};
        parse_and_expect_types("2;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NNumber, NEnd};
        parse_and_expect_types("2;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NAnd, NTrue, NTrue, NEnd};
        parse_and_expect_types("true && true;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NOr, NTrue, NTrue, NEnd};
        parse_and_expect_types("true || true;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NOr, NTrue, NOr, NTrue, NTrue, NEnd};
        parse_and_expect_types("true || true || true;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NXor, NTrue, NTrue, NEnd};
        parse_and_expect_types("true ^ true;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NBitAnd, NNumber, NNumber, NEnd};
        parse_and_expect_types("0 & 1;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NBitOr, NNumber, NNumber, NEnd};
        parse_and_expect_types("0 | 1;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NIdentifier, NEnd};
        parse_and_expect_types("foo;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NPreIncrement, NIdentifier, NEnd};
        parse_and_expect_types("++foo;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NPostIncrement, NIdentifier, NEnd};
        parse_and_expect_types("foo++;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NNot, NTrue, NEnd};
        parse_and_expect_types("!true;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NNegative, NNumber, NEnd};
        parse_and_expect_types("-1;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NNumber, NEnd};
        parse_and_expect_types("+1;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NNumber, NEnd};
        parse_and_expect_types("const foo = +1;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NIdentifier, NEnd};
        parse_and_expect_types("foo;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NCall, NIdentifier, NCallArgs, NEnd};
        parse_and_expect_types("foo();", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NAccess, NIdentifier, NIdentifier, NEnd};
        parse_and_expect_types("foo.bar;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NCall, NAccess, NIdentifier, NIdentifier, NCallArgs, NEnd};
        parse_and_expect_types("foo.bar();", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NCall, NAccess, NCall, NIdentifier, NCallArgs, NIdentifier, NCallArgs, NEnd};
        parse_and_expect_types("foo().bar();", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NAccess, NCall, NIdentifier, NCallArgs, NIdentifier, NEnd};
        parse_and_expect_types("foo().bar;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NNumber, NEnd};
        parse_and_expect_types("const foo = 2;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NStringLiteral, NEnd};
        parse_and_expect_types("const foo = \"Hello\";", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NVarDeclaration, NIdentifier, NTrue, NEnd};
        parse_and_expect_types("var foo = true;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NTypeDeclaration, NTypeIdentifier, NTypeIdentifier, NEnd};
        parse_and_expect_types("type foo = u8;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NTypeDeclaration, NTypeIdentifier, NTypeIdentifier, NEnd};
        parse_and_expect_types("type foo = u8;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NTypeDeclaration, NTypeIdentifier, NArray, NTypeIdentifier, NEnd};
        parse_and_expect_types("type foo = u8[];", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NTypeDeclaration, NTypeIdentifier, NStruct, NStructProperty, NIdentifier, NTypeIdentifier, NEnd};
        parse_and_expect_types("type foo = { bar: u8 };", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NTypeDeclaration, NTypeIdentifier, NStruct, NStructProperty, NIdentifier, NTypeIdentifier, NStructProperty, NIdentifier, NTypeIdentifier, NEnd};
        parse_and_expect_types("type foo = { bar: u8, goo: u32 };", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NTypeDeclaration, NTypeIdentifier, NStruct, NStructProperty, NIdentifier, NArray, NTypeIdentifier, NEnd};
        parse_and_expect_types("type foo = { bar: u8[] };", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NTypeDeclaration, NTypeIdentifier, NStruct, NStructProperty, NIdentifier, NStruct, NEnd};
        parse_and_expect_types("type foo = { bar: {} };", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NTypeDeclaration, NTypeIdentifier, NStruct, NStructProperty, NIdentifier, NStruct, NStructProperty, NIdentifier, NTypeIdentifier, NEnd};
        parse_and_expect_types("type foo = { bar: { bar: u8 } };", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NStruct, NStructProperty, NIdentifier, NInt32, NEnd};
        parse_and_expect_types("const foo = { bar: 1i32 };", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NStruct, NStructProperty, NIdentifier, NUint16, NEnd};
        parse_and_expect_types("const foo = { bar: 3u16 };", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NStruct, NStructProperty, NIdentifier, NDecimal, NStructProperty, NIdentifier, NDecimal, NEnd};
        parse_and_expect_types("const foo = { bar: 1.0, goo: 2.0 };", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NStruct, NStructProperty, NIdentifier, NStruct, NEnd};
        parse_and_expect_types("const foo = { bar: {} };", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NStruct, NStructProperty, NIdentifier, NStruct, NStructProperty, NIdentifier, NTrue, NEnd};
        parse_and_expect_types("const foo = { bar: { bar: true } };", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NReassignment, NIdentifier, NNumber, NEnd};
        parse_and_expect_types("foo = 123;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStructDeclaration, NTypeIdentifier, NEnd};
        parse_and_expect_types("struct Position {};", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStructDeclaration, NTypeIdentifier, NStructProperty, NIdentifier, NTypeIdentifier, NEnd};
        parse_and_expect_types("struct S { x: u8 };", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NTypeIdentifier, NNumber, NEnd};
        parse_and_expect_types("const foo: u8 = 10;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NFunction, NFunctionArgs, NNumber, NEnd};
        parse_and_expect_types("const foo = fn () => 10;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NNumber, NConstDeclaration, NIdentifier, NFunction, NFunctionArgs, NNumber, NEnd};
        parse_and_expect_types("10; const foo = fn () => 10;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NFunction, NFunctionArgs, NIdentifier, NEnd};
        parse_and_expect_types("const foo = fn () => x;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NFunction, NFunctionArgs, NFunctionArg, NIdentifier, NIdentifier, NEnd};
        parse_and_expect_types("const foo = fn (x) => x;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NFunction, NFunctionArgs, NFunctionArg, NIdentifier, NTypeIdentifier, NIdentifier, NEnd};
        parse_and_expect_types("const foo = fn (x: u8) => x;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NFunction, NFunctionArgs, NTypeIdentifier, NIdentifier, NEnd};
        parse_and_expect_types("const foo = fn (): u32 => x;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NAdd, NNumber, NNumber, NEnd};
        parse_and_expect_types("const foo = 1 + 2;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NAdd, NMul, NNumber, NNumber, NNumber, NEnd};
        parse_and_expect_types("const foo = 1 * 2 + 3;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NAdd, NNumber, NMul, NNumber, NNumber, NEnd};
        parse_and_expect_types("const foo = 1 + 2 * 3;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NEq, NNot, NTrue, NFalse, NEnd};
        parse_and_expect_types("const foo = !true == false;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NGe, NAdd, NNumber, NNumber, NNumber, NEnd};
        parse_and_expect_types("const foo = 1 + 2 >= 0;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NIf, NTrue, NBlock, NEnd};
        parse_and_expect_types("if (true) {} ", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NIf, NTrue, NBlock, NStatement, NNumber, NEnd};
        parse_and_expect_types("if (true) { 1; }", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NIf, NTrue, NBlock, NStatement, NNumber, NElse, NBlock, NEnd};
        parse_and_expect_types("if (true) { 1; } else {}", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NIf, NTrue, NBlock, NStatement, NNumber, NElse, NBlock, NStatement, NNumber, NEnd};
        parse_and_expect_types("if (true) { 1; } else { 1; }", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NCall, NIdentifier, NCallArgs, NEnd};
        parse_and_expect_types("const foo = bar();", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NCall, NIdentifier, NCallArgs, NCallArg, NNumber, NEnd};
        parse_and_expect_types("const foo = bar(12);", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NCall, NIdentifier, NCallArgs, NCallArg, NIdentifier, NNumber, NEnd};
        parse_and_expect_types("const foo = bar(x: 12);", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NAccess, NIdentifier, NIdentifier, NEnd};
        parse_and_expect_types("const foo = foo.bar;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NAccess, NAccess, NIdentifier, NIdentifier, NIdentifier, NEnd};
        parse_and_expect_types("const foo = foo.bar.test;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NFunction, NFunctionArgs, NTypeIdentifier, NBlock, NReturn, NNumber, NEnd};
        parse_and_expect_types("const foo = fn (): u8 => {\n\treturn 10;\n};", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NComment, NStatement, NNumber, NEnd};
        parse_and_expect_types("// Comment\n123;\n", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NStringInterpolation, NIdentifier, NStringInterpolation, NEnd};
        parse_and_expect_types("\"{foo}\";", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NStringInterpolation, NIdentifier, NIdentifier, NStringInterpolation, NEnd};
        parse_and_expect_types("\"{foo}{bar}\";", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NStringInterpolation, NIdentifier, NStringInterpolation, NIdentifier, NStringInterpolation, NEnd};
        parse_and_expect_types("\"{foo} {bar}\";", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NFunctionDeclaration, NIdentifier, NFunctionArgs, NTypeIdentifier, NBlock, NReturn, NNumber, NEnd};
        parse_and_expect_types(
            "fn main(): u8 {\n"
            "  return 10;\n"
            "}",
            expected);
    }
    {
        enum NodeType expected[] = {NProgram, NFor, NVarDeclaration, NIdentifier, NNumber, NSt, NIdentifier, NNumber, NPostIncrement, NIdentifier, NBlock, NEnd};
        parse_and_expect_types("for (var i = 0; i < 10; i++) {}", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NWhile, NTrue, NBlock, NEnd};
        parse_and_expect_types("while (true) {} ", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NWhile, NTrue, NBlock, NBreak, NEnd};
        parse_and_expect_types("while (true) { break; } ", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NWhile, NTrue, NBlock, NContinue, NEnd};
        parse_and_expect_types("while (true) { continue; } ", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NPipe, NNumber, NIdentifier, NEnd};
        parse_and_expect_types("10 |> double;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NPipe, NNumber, NIdentifier, NIdentifier, NEnd};
        parse_and_expect_types("10 |> double |> triple;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NPipe, NNumber, NFunction, NFunctionArgs, NFunctionArg, NIdentifier, NIdentifier, NEnd};
        parse_and_expect_types("10 |> fn x => x;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NStatement, NPipe, NNumber, NFunction, NFunctionArgs, NFunctionArg, NIdentifier, NPipe, NIdentifier, NIdentifier, NEnd};
        parse_and_expect_types("10 |> fn x => x |> double;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NConstDeclaration, NIdentifier, NTypeIdentifier, NTypeParameters, NTypeIdentifier, NArray, NEnd};
        parse_and_expect_types("const x: Array<u8> = [];", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NTypeDeclaration, NTypeIdentifier, NTypeParameters, NTypeIdentifier, NTypeIdentifier, NEnd};
        parse_and_expect_types("type Array<T> = T;", expected);
    }
    {
        enum NodeType expected[] = {NProgram, NFunctionDeclaration, NIdentifier, NFunctionArgs, NTypeIdentifier, NBlock, NEnd};
        parse_and_expect_types("fn main() {\n\n}\n", expected);
    }
}
