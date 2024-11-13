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

typedef struct TestAcc
{
    bool success;
} TestAcc;

bool traverse_expect(Node *node, TraverseAcc *acc)
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

void test(TestAcc *test_acc, char *src, enum NodeType *expectations)
{
    int test = ++counter;
    TraverseAcc acc = {0, expectations};
    struct Node *node = parse(src);
    if (traverse_expect(node, &acc) == false)
    {
        printf("Failed test %d, with src: %s", test, src);
        printf("\n\nTokens:\n\n");
        print_tokens(src);
        printf("\n\nAST:\n\n");
        print_ast(src);
        test_acc->success = false;
    }
}

int main()
{
    TestAcc acc = (TestAcc){.success = true};

    test(&acc, "2;", (enum NodeType[]){NProgram, NStatement, NNumber, NEnd});
    test(&acc, "2;", (enum NodeType[]){NProgram, NStatement, NNumber, NEnd});
    test(&acc, "true && true;", (enum NodeType[]){NProgram, NStatement, NAnd, NTrue, NTrue, NEnd});
    test(&acc, "true || true;", (enum NodeType[]){NProgram, NStatement, NOr, NTrue, NTrue, NEnd});
    test(&acc, "true || true || true;", (enum NodeType[]){NProgram, NStatement, NOr, NTrue, NOr, NTrue, NTrue, NEnd});
    test(&acc, "true ^ true;", (enum NodeType[]){NProgram, NStatement, NXor, NTrue, NTrue, NEnd});
    test(&acc, "0 & 1;", (enum NodeType[]){NProgram, NStatement, NBitAnd, NNumber, NNumber, NEnd});
    test(&acc, "0 | 1;", (enum NodeType[]){NProgram, NStatement, NBitOr, NNumber, NNumber, NEnd});
    test(&acc, "foo;", (enum NodeType[]){NProgram, NStatement, NIdentifier, NEnd});
    test(&acc, "++foo;", (enum NodeType[]){NProgram, NStatement, NPreIncrement, NIdentifier, NEnd});
    test(&acc, "foo++;", (enum NodeType[]){NProgram, NStatement, NPostIncrement, NIdentifier, NEnd});
    test(&acc, "!true;", (enum NodeType[]){NProgram, NStatement, NNot, NTrue, NEnd});
    test(&acc, "-1;", (enum NodeType[]){NProgram, NStatement, NNegative, NNumber, NEnd});
    test(&acc, "+1;", (enum NodeType[]){NProgram, NStatement, NNumber, NEnd});
    test(&acc, "const foo = +1;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NNumber, NEnd});
    test(&acc, "foo;", (enum NodeType[]){NProgram, NStatement, NIdentifier, NEnd});
    test(&acc, "foo();", (enum NodeType[]){NProgram, NStatement, NCall, NIdentifier, NCallArgs, NEnd});
    test(&acc, "foo.bar;", (enum NodeType[]){NProgram, NStatement, NAccess, NIdentifier, NIdentifier, NEnd});
    test(&acc, "foo.bar();", (enum NodeType[]){NProgram, NStatement, NCall, NAccess, NIdentifier, NIdentifier, NCallArgs, NEnd});
    test(&acc, "foo().bar();", (enum NodeType[]){NProgram, NStatement, NCall, NAccess, NCall, NIdentifier, NCallArgs, NIdentifier, NCallArgs, NEnd});
    test(&acc, "foo().bar;", (enum NodeType[]){NProgram, NStatement, NAccess, NCall, NIdentifier, NCallArgs, NIdentifier, NEnd});
    test(&acc, "const foo = 2;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NNumber, NEnd});
    test(&acc, "const foo = \"Hello\";", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NStringLiteral, NEnd});
    test(&acc, "var foo = true;", (enum NodeType[]){NProgram, NVarDeclaration, NIdentifier, NTrue, NEnd});
    test(&acc, "type foo = u8;", (enum NodeType[]){NProgram, NTypeDeclaration, NTypeIdentifier, NTypeIdentifier, NEnd});
    test(&acc, "type foo = u8;", (enum NodeType[]){NProgram, NTypeDeclaration, NTypeIdentifier, NTypeIdentifier, NEnd});
    test(&acc, "type foo = u8[];", (enum NodeType[]){NProgram, NTypeDeclaration, NTypeIdentifier, NArray, NTypeIdentifier, NEnd});
    test(&acc, "type foo = { bar: u8 };", (enum NodeType[]){NProgram, NTypeDeclaration, NTypeIdentifier, NStructType, NStructProperty, NIdentifier, NTypeIdentifier, NEnd});
    test(&acc, "type foo = { bar: u8, goo: u32 };", (enum NodeType[]){NProgram, NTypeDeclaration, NTypeIdentifier, NStructType, NStructProperty, NIdentifier, NTypeIdentifier, NStructProperty, NIdentifier, NTypeIdentifier, NEnd});
    test(&acc, "type foo = { bar: u8[] };", (enum NodeType[]){NProgram, NTypeDeclaration, NTypeIdentifier, NStructType, NStructProperty, NIdentifier, NArray, NTypeIdentifier, NEnd});
    test(&acc, "type foo = { bar: {} };", (enum NodeType[]){NProgram, NTypeDeclaration, NTypeIdentifier, NStructType, NStructProperty, NIdentifier, NStructType, NEnd});
    test(&acc, "type foo = { bar: { bar: u8 } };", (enum NodeType[]){NProgram, NTypeDeclaration, NTypeIdentifier, NStructType, NStructProperty, NIdentifier, NStructType, NStructProperty, NIdentifier, NTypeIdentifier, NEnd});
    test(&acc, "const foo = { bar: 1i32 };", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NStruct, NStructProperty, NIdentifier, NInt32, NEnd});
    test(&acc, "const foo = { bar: 3u16 };", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NStruct, NStructProperty, NIdentifier, NUint16, NEnd});
    test(&acc, "const foo = { bar: 1.0, goo: 2.0 };", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NStruct, NStructProperty, NIdentifier, NDecimal, NStructProperty, NIdentifier, NDecimal, NEnd});
    test(&acc, "const foo = { bar: {} };", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NStruct, NStructProperty, NIdentifier, NStruct, NEnd});
    test(&acc, "const foo = { bar: { bar: true } };", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NStruct, NStructProperty, NIdentifier, NStruct, NStructProperty, NIdentifier, NTrue, NEnd});
    test(&acc, "foo = 123;", (enum NodeType[]){NProgram, NReassignment, NIdentifier, NNumber, NEnd});
    test(&acc, "struct Position {};", (enum NodeType[]){NProgram, NStructDeclaration, NTypeIdentifier, NEnd});
    test(&acc, "struct S { x: u8 };", (enum NodeType[]){NProgram, NStructDeclaration, NTypeIdentifier, NStructProperty, NIdentifier, NTypeIdentifier, NEnd});
    test(&acc, "const foo: u8 = 10;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NTypeIdentifier, NNumber, NEnd});
    test(&acc, "const foo = fn () => 10;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NFunction, NFunctionArgs, NNumber, NEnd});
    test(&acc, "10; const foo = fn () => 10;", (enum NodeType[]){NProgram, NStatement, NNumber, NConstDeclaration, NIdentifier, NFunction, NFunctionArgs, NNumber, NEnd});
    test(&acc, "const foo = fn () => x;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NFunction, NFunctionArgs, NIdentifier, NEnd});
    test(&acc, "const foo = fn (x) => x;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NFunction, NFunctionArgs, NFunctionArg, NIdentifier, NIdentifier, NEnd});
    test(&acc, "const foo = fn (x: u8) => x;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NFunction, NFunctionArgs, NFunctionArg, NIdentifier, NTypeIdentifier, NIdentifier, NEnd});
    test(&acc, "const foo = fn (): u32 => x;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NFunction, NFunctionArgs, NTypeIdentifier, NIdentifier, NEnd});
    test(&acc, "const foo = 1 + 2;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NAdd, NNumber, NNumber, NEnd});
    test(&acc, "const foo = 1 * 2 + 3;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NAdd, NMul, NNumber, NNumber, NNumber, NEnd});
    test(&acc, "const foo = 1 + 2 * 3;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NAdd, NNumber, NMul, NNumber, NNumber, NEnd});
    test(&acc, "const foo = !true == false;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NEq, NNot, NTrue, NFalse, NEnd});
    test(&acc, "const foo = 1 + 2 >= 0;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NGe, NAdd, NNumber, NNumber, NNumber, NEnd});
    test(&acc, "if (true) {} ", (enum NodeType[]){NProgram, NIf, NTrue, NBlock, NEnd});
    test(&acc, "if (true) { 1; }", (enum NodeType[]){NProgram, NIf, NTrue, NBlock, NStatement, NNumber, NEnd});
    test(&acc, "if (true) { 1; } else {}", (enum NodeType[]){NProgram, NIf, NTrue, NBlock, NStatement, NNumber, NElse, NBlock, NEnd});
    test(&acc, "if (true) { 1; } else { 1; }", (enum NodeType[]){NProgram, NIf, NTrue, NBlock, NStatement, NNumber, NElse, NBlock, NStatement, NNumber, NEnd});
    test(&acc, "const foo = bar();", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NCall, NIdentifier, NCallArgs, NEnd});
    test(&acc, "const foo = bar(12);", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NCall, NIdentifier, NCallArgs, NCallArg, NNumber, NEnd});
    test(&acc, "const foo = bar(x: 12);", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NCall, NIdentifier, NCallArgs, NCallArg, NIdentifier, NNumber, NEnd});
    test(&acc, "const foo = foo.bar;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NAccess, NIdentifier, NIdentifier, NEnd});
    test(&acc, "const foo = foo.bar.test;", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NAccess, NAccess, NIdentifier, NIdentifier, NIdentifier, NEnd});
    test(&acc, "const foo = fn (): u8 => {\n\treturn 10;\n};", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NFunction, NFunctionArgs, NTypeIdentifier, NBlock, NReturn, NNumber, NEnd});
    test(&acc, "// Comment\n123;\n", (enum NodeType[]){NProgram, NComment, NStatement, NNumber, NEnd});
    test(&acc, "\"{foo}\";", (enum NodeType[]){NProgram, NStatement, NStringInterpolation, NIdentifier, NStringInterpolation, NEnd});
    test(&acc, "\"{foo}{bar}\";", (enum NodeType[]){NProgram, NStatement, NStringInterpolation, NIdentifier, NIdentifier, NStringInterpolation, NEnd});
    test(&acc, "\"{foo} {bar}\";", (enum NodeType[]){NProgram, NStatement, NStringInterpolation, NIdentifier, NStringInterpolation, NIdentifier, NStringInterpolation, NEnd});
    test(&acc, "for (var i = 0; i < 10; i++) {}", (enum NodeType[]){NProgram, NFor, NVarDeclaration, NIdentifier, NNumber, NSt, NIdentifier, NNumber, NPostIncrement, NIdentifier, NBlock, NEnd});
    test(&acc, "while (true) {} ", (enum NodeType[]){NProgram, NWhile, NTrue, NBlock, NEnd});
    test(&acc, "while (true) { break; } ", (enum NodeType[]){NProgram, NWhile, NTrue, NBlock, NBreak, NEnd});
    test(&acc, "while (true) { continue; } ", (enum NodeType[]){NProgram, NWhile, NTrue, NBlock, NContinue, NEnd});
    test(&acc, "10 |> double;", (enum NodeType[]){NProgram, NStatement, NPipe, NNumber, NIdentifier, NEnd});
    test(&acc, "10 |> double |> triple;", (enum NodeType[]){NProgram, NStatement, NPipe, NNumber, NIdentifier, NIdentifier, NEnd});
    test(&acc, "10 |> fn x => x;", (enum NodeType[]){NProgram, NStatement, NPipe, NNumber, NFunction, NFunctionArgs, NFunctionArg, NIdentifier, NIdentifier, NEnd});
    test(&acc, "10 |> fn x => x |> double;", (enum NodeType[]){NProgram, NStatement, NPipe, NNumber, NFunction, NFunctionArgs, NFunctionArg, NIdentifier, NPipe, NIdentifier, NIdentifier, NEnd});
    test(&acc, "const x: Array<u8> = [];", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NTypeIdentifier, NTypeParameters, NTypeIdentifier, NArray, NEnd});
    test(&acc, "type Array<T> = T;", (enum NodeType[]){NProgram, NTypeDeclaration, NTypeIdentifier, NTypeParameters, NTypeIdentifier, NTypeIdentifier, NEnd});
    test(&acc, "fn main() {\n\n}\n", (enum NodeType[]){NProgram, NFunctionDeclaration, NIdentifier, NFunctionArgs, NTypeIdentifier, NBlock, NEnd});
    test(
        &acc,
        "fn main(): u8 {\n"
        "  return 10;\n"
        "}",
        (enum NodeType[]){NProgram, NFunctionDeclaration, NIdentifier, NFunctionArgs, NTypeIdentifier, NBlock, NReturn, NNumber, NEnd});
    test(&acc, "const a = @malloc A({});", (enum NodeType[]){NProgram, NConstDeclaration, NIdentifier, NEffect, NEffectIdentifier, NCall, NIdentifier, NCallArgs, NCallArg, NStruct, NEnd});

    if (acc.success)
    {
        printf("\nAll tests passed");
    }
}
