#include "./lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void tokenize_and_get_types(char *src, enum TokenType *expected, int count)
{
    Token *token = tokenize(src);
    token = token->next;

    for (int i = 0; i < count; i++)
    {
        if (token->type != *expected)
        {
            print_tokens(src);
            printf("\033[31;1mError:\033[0m expected %s but got \033[31;1m%s\033[0m\n", get_token_name(*expected), get_token_name(token->type));
        }

        token = token->next;
        expected++;
    }
}

void tokenize_and_get_values(char *src, char *expected[], int count)
{
    Token *token = tokenize(src);
    token = token->next;

    for (int i = 0; i < count; i++)
    {
        if (strcmp(token->content, *expected) != 0)
        {
            printf("Token: %s\n", token->content);
            printf("\033[31;1mError:\033[0m expected %s but got \033[31;1m%s\033[0m\n", *expected, token->content);
        }

        token = token->next;
        expected++;
    }
}

int main()
{
    {
        enum TokenType expected[] = {TNumber};
        tokenize_and_get_types("1", expected, 1);
    }
    {
        char *expected[] = {"1"};
        tokenize_and_get_values("1", expected, 1);
    }
    {
        enum TokenType expected[] = {TDecimal};
        tokenize_and_get_types("1.0", expected, 1);
    }
    {
        enum TokenType expected[] = {TFloat32};
        tokenize_and_get_types("1.0f32", expected, 1);
    }
    {
        enum TokenType expected[] = {TFloat64};
        tokenize_and_get_types("1f64", expected, 1);
    }
    {
        enum TokenType expected[] = {TInt8};
        tokenize_and_get_types("1i8", expected, 1);
    }
    {
        enum TokenType expected[] = {TInt16};
        tokenize_and_get_types("2i16", expected, 1);
    }
    {
        enum TokenType expected[] = {TInt32};
        tokenize_and_get_types("1i32", expected, 1);
    }
    {
        enum TokenType expected[] = {TInt64};
        tokenize_and_get_types("10i64", expected, 1);
    }
    {
        enum TokenType expected[] = {TUint8};
        tokenize_and_get_types("1u8", expected, 1);
    }
    {
        enum TokenType expected[] = {TUint16};
        tokenize_and_get_types("2u16", expected, 1);
    }
    {
        enum TokenType expected[] = {TUint32};
        tokenize_and_get_types("1u32", expected, 1);
    }
    {
        enum TokenType expected[] = {TUint64};
        tokenize_and_get_types("10u64", expected, 1);
    }
    {
        enum TokenType expected[] = {TDecimal};
        tokenize_and_get_types("1.0", expected, 1);
    }
    {
        enum TokenType expected[] = {TTrue};
        tokenize_and_get_types("true", expected, 1);
    }
    {
        enum TokenType expected[] = {TFalse};
        tokenize_and_get_types("false", expected, 1);
    }
    {
        enum TokenType expected[] = {TIdentifier};
        tokenize_and_get_types("foo", expected, 1);
    }
    {
        enum TokenType expected[] = {TEffectIdentifier};
        tokenize_and_get_types("@foo", expected, 1);
    }
    {
        enum TokenType expected[] = {TIdentifier};
        tokenize_and_get_types("foo", expected, 1);
    }
    {
        enum TokenType expected[] = {TAdd};
        tokenize_and_get_types("+", expected, 1);
    }
    {
        enum TokenType expected[] = {TSub};
        tokenize_and_get_types("-", expected, 1);
    }
    {
        enum TokenType expected[] = {TMul};
        tokenize_and_get_types("*", expected, 1);
    }
    {
        enum TokenType expected[] = {TDiv};
        tokenize_and_get_types("/", expected, 1);
    }
    {
        enum TokenType expected[] = {TEq};
        tokenize_and_get_types("==", expected, 1);
    }
    {
        enum TokenType expected[] = {TNeq};
        tokenize_and_get_types("!=", expected, 1);
    }
    {
        enum TokenType expected[] = {TGt};
        tokenize_and_get_types(">", expected, 1);
    }
    {
        enum TokenType expected[] = {TSt};
        tokenize_and_get_types("<", expected, 1);
    }
    {
        enum TokenType expected[] = {TGe};
        tokenize_and_get_types(">=", expected, 1);
    }
    {
        enum TokenType expected[] = {TSe};
        tokenize_and_get_types("<=", expected, 1);
    }
    {
        enum TokenType expected[] = {TAnd};
        tokenize_and_get_types("&&", expected, 1);
    }
    {
        enum TokenType expected[] = {TOr};
        tokenize_and_get_types("||", expected, 1);
    }
    {
        enum TokenType expected[] = {TBitAnd};
        tokenize_and_get_types("&", expected, 1);
    }
    {
        enum TokenType expected[] = {TBitOr};
        tokenize_and_get_types("|", expected, 1);
    }
    {
        enum TokenType expected[] = {TXor};
        tokenize_and_get_types("^", expected, 1);
    }
    {
        enum TokenType expected[] = {TAssign};
        tokenize_and_get_types("=", expected, 1);
    }
    {
        enum TokenType expected[] = {TOpenParens};
        tokenize_and_get_types("(", expected, 1);
    }
    {
        enum TokenType expected[] = {TCloseParens};
        tokenize_and_get_types(")", expected, 1);
    }
    {
        enum TokenType expected[] = {TIncrement};
        tokenize_and_get_types("++", expected, 1);
    }
    {
        enum TokenType expected[] = {TDecrement};
        tokenize_and_get_types("--", expected, 1);
    }
    {
        enum TokenType expected[] = {TNot};
        tokenize_and_get_types("!", expected, 1);
    }
    {
        enum TokenType expected[] = {TString};
        tokenize_and_get_types("\"Hello\"", expected, 1);
    }
    {
        enum TokenType expected[] = {TOpenCurlyBracket};
        tokenize_and_get_types("{", expected, 1);
    }
    {
        enum TokenType expected[] = {TCloseCurlyBracket};
        tokenize_and_get_types("}", expected, 1);
    }
    {
        enum TokenType expected[] = {TOpenSquareBracket};
        tokenize_and_get_types("[", expected, 1);
    }
    {
        enum TokenType expected[] = {TCloseSquareBracket};
        tokenize_and_get_types("]", expected, 1);
    }
    {
        enum TokenType expected[] = {TColon};
        tokenize_and_get_types(":", expected, 1);
    }
    {
        enum TokenType expected[] = {TIf};
        tokenize_and_get_types("if", expected, 1);
    }
    {
        enum TokenType expected[] = {TElse};
        tokenize_and_get_types("else", expected, 1);
    }
    {
        enum TokenType expected[] = {TStruct};
        tokenize_and_get_types("struct", expected, 1);
    }
    {
        enum TokenType expected[] = {TUnion};
        tokenize_and_get_types("union", expected, 1);
    }
    {
        enum TokenType expected[] = {TMatch};
        tokenize_and_get_types("match", expected, 1);
    }
    {
        enum TokenType expected[] = {TFnArrow};
        tokenize_and_get_types("=>", expected, 1);
    }
    {
        enum TokenType expected[] = {TPlaceholder};
        tokenize_and_get_types("_", expected, 1);
    }
    {
        enum TokenType expected[] = {TRest};
        tokenize_and_get_types("..", expected, 1);
    }
    {
        enum TokenType expected[] = {TComment};
        tokenize_and_get_types("// hello", expected, 1);
    }
    {
        enum TokenType expected[] = {TIdentifier, TAssign, TString, TEnd};
        tokenize_and_get_types("foo = \"Hello\"", expected, 4);
    }
    {
        enum TokenType expected[] = {TIdentifier, TEnd};
        tokenize_and_get_types("u8", expected, 2);
    }
    {
        enum TokenType expected[] = {TConst, TIdentifier, TEnd};
        tokenize_and_get_types("const foo", expected, 2);
    }
    {
        enum TokenType expected[] = {TVar, TIdentifier, TEnd};
        tokenize_and_get_types("var foo", expected, 2);
    }
    {
        enum TokenType expected[] = {TType, TIdentifier, TEnd};
        tokenize_and_get_types("type foo", expected, 2);
    }
    {
        enum TokenType expected[] = {TStringInterpolation, TIdentifier, TStringInterpolation};
        tokenize_and_get_types("\"Hello {foo}\"", expected, 3);
    }
    {
        enum TokenType expected[] = {TStringInterpolation, TIdentifier, TStringInterpolation};
        tokenize_and_get_types("\"Hello foo{bar} you\"", expected, 3);
    }
    {
        enum TokenType expected[] = {TStringInterpolation, TIdentifier, TIdentifier, TStringInterpolation};
        tokenize_and_get_types("\"Hello {foo}{bar}\"", expected, 4);
    }
    {
        enum TokenType expected[] = {TStringInterpolation, TIdentifier, TIdentifier, TStringInterpolation};
        tokenize_and_get_types("\"{foo}{bar}\"", expected, 4);
    }
    {
        enum TokenType expected[] = {TStringInterpolation, TIdentifier, TStringInterpolation, TIdentifier, TStringInterpolation};
        tokenize_and_get_types("\"{foo} {bar}\"", expected, 5);
    }
    {
        enum TokenType expected[] = {TStringInterpolation, TIdentifier, TStringInterpolation, TEnd};
        tokenize_and_get_types("\"{foo}\"", expected, 4);
    }
    {
        enum TokenType expected[] = {TNumber, TSemicolon};
        tokenize_and_get_types("2;", expected, 2);
    }
    {
        enum TokenType expected[] = {TNamespaceIdentifier, TEnd};
        tokenize_and_get_types("#foo", expected, 2);
    }
    {
        enum TokenType expected[] = {TWith, TEffectIdentifier};
        tokenize_and_get_types("with @zone", expected, 2);
    }
    {
        enum TokenType expected[] = {TBreak, TEnd};
        tokenize_and_get_types("break", expected, 2);
    }
    {
        enum TokenType expected[] = {TContinue, TEnd};
        tokenize_and_get_types("continue", expected, 2);
    }
    {
        enum TokenType expected[] = {TFor, TEnd};
        tokenize_and_get_types("for", expected, 2);
    }
    {
        enum TokenType expected[] = {TWhile, TEnd};
        tokenize_and_get_types("while", expected, 2);
    }
    {
        enum TokenType expected[] = {TWith, TEnd};
        tokenize_and_get_types("with", expected, 2);
    }
}
