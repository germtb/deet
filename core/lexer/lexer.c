#include "./lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char *get_token_name(enum TokenType type)
{
    switch (type)
    {
    case TStart:
        return "Start";
    case TFn:
        return "Fn";
    case TNumber:
        return "Number";
    case TWith:
        return "With";
    case TConst:
        return "Const";
    case TVar:
        return "Var";
    case TType:
        return "Type";
    case TTrue:
        return "True";
    case TFalse:
        return "False";
    case TIdentifier:
        return "Identifier";
    case TEffectIdentifier:
        return "EffectIdentifier";
    case TAdd:
        return "Add";
    case TSub:
        return "Sub";
    case TMul:
        return "Mul";
    case TDiv:
        return "Div";
    case TEq:
        return "Eq";
    case TNeq:
        return "Neq";
    case TGt:
        return "Gt";
    case TSt:
        return "St";
    case TGe:
        return "Ge";
    case TSe:
        return "Se";
    case TAnd:
        return "And";
    case TOr:
        return "Or";
    case TBitAnd:
        return "BitAnd";
    case TBitOr:
        return "BitOr";
    case TXor:
        return "Xor";
    case TAssign:
        return "Assign";
    case TOpenParens:
        return "OpenParens";
    case TCloseParens:
        return "CloseParens";
    case TIncrement:
        return "Increment";
    case TDecrement:
        return "Decrement";
    case TNot:
        return "Not";
    case TString:
        return "String";
    case TReturn:
        return "Return";
    case TPipe:
        return "Pipe";
    case TStringInterpolation:
        return "StringInterpolation";
    case TSemicolon:
        return "Semicolon";
    case TComma:
        return "Comma";
    case TEffect:
        return "Effect";
    case TFor:
        return "For";
    case TContinue:
        return "Continue";
    case TBreak:
        return "Break";
    case TWhile:
        return "While";
    case TOpenCurlyBracket:
        return "OpenCurlyBracket";
    case TCloseCurlyBracket:
        return "CloseCurlyBracket";
    case TOpenSquareBracket:
        return "OpenSquareBracket";
    case TCloseSquareBracket:
        return "CloseSquareBracket";
    case TColon:
        return "Colon";
    case TIf:
        return "If";
    case TElse:
        return "Else";
    case TStruct:
        return "Struct";
    case TUnion:
        return "Union";
    case TAccess:
        return "Access";
    case TMatch:
        return "Match";
    case TFnArrow:
        return "FnArrow";
    case TPlaceholder:
        return "Placeholder";
    case TRest:
        return "Rest";
    case TEnd:
        return "End";
    case TComment:
        return "Comment";
    case TInt8:
        return "Int8";
    case TInt16:
        return "Int16";
    case TInt32:
        return "Int32";
    case TInt64:
        return "Int64";
    case TUint8:
        return "Uint8";
    case TUint16:
        return "Uint16";
    case TUint32:
        return "Uint32";
    case TUint64:
        return "Uint64";
    case TFloat32:
        return "Float32";
    case TFloat64:
        return "Float64";
    case TDecimal:
        return "Decimal";
    case TNamespace:
        return "Namespace";
    case TNamespaceIdentifier:
        return "NamespaceIdentifier";
    default:
        return "Invalid";
    }
}

bool is_letter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_number(char c)
{
    return (c >= '0' && c <= '9');
}

bool is_valid_identifier_part(char c)
{
    return is_letter(c) || c == '_' || is_number(c);
}

bool is_whitespace(char c)
{
    return c == ' ' || c == '\n' || c == '\t' || c == '\0';
}

bool is_lineend(char c)
{
    return c == '\n' || c == '\0';
}

enum AccType
{
    Empty,
    InNumber,
    InUint,
    InInt,
    InDecimal,
    InFloat,
    InString,
    InStringInterpolatedValue,
    InStringInterpolation,
    InComment,
    InNamespaceIdentifier,
    InIdentifier,
    InEffectIdentifier,
};

struct Acc
{
    enum AccType type;
    char content[100];
    int len;
    char *src;
    Token *list;
};

void reset_acc(struct Acc *acc)
{
    acc->type = Empty;
    acc->len = 0;
    acc->content[0] = '\0';
}

char read_next(struct Acc *acc)
{
    char c = *acc->src;
    acc->src++;
    acc->content[acc->len] = c;
    acc->len++;
    return c;
}

char read_peek(struct Acc *acc)
{
    return *acc->src;
}

void make_token(enum TokenType type, struct Acc *acc, int trim)
{
    // Trim the content
    int len = acc->len - trim;

    // Copy the content
    char *content = malloc((len + 1) * sizeof(char));
    strncpy(content, acc->content, len);
    content[len] = '\0';

    // New token
    Token *next = malloc(sizeof(Token));
    next->type = type;
    next->content = content;
    acc->list->next = next;
    acc->list = next;

    reset_acc(acc);

    // printf("%s(%s)\n", get_token_name(token->type), token->content);
}

bool is_prefix(char *str, char *prefix)
{
    return strncmp(prefix, str, strlen(prefix)) == 0;
}

char *get_acc_type(enum AccType acc_type)
{
    switch (acc_type)
    {
    case Empty:
        return "Empty";
    case InNumber:
        return "InNumber";
    case InUint:
        return "InUint";
    case InInt:
        return "InInt";
    case InFloat:
        return "InFloat";
    case InDecimal:
        return "InDecimal";
    case InString:
        return "InString";
    case InStringInterpolatedValue:
        return "InStringInterpolatedValue";
    case InStringInterpolation:
        return "InStringInterpolation";
    case InComment:
        return "InComment";
    case InNamespaceIdentifier:
        return "InNamespaceIdentifier";
    case InIdentifier:
        return "InIdentifier";
    case InEffectIdentifier:
        return "InEffectIdentifier";
    default:
        return "InvalidAccType";
    }
}

Token *tokenize(char *src)
{

    Token *head = malloc(sizeof(Token));
    head->type = TStart;
    head->content = "";
    head->next = NULL;

    struct Acc acc = {.type = Empty, .content = "", .len = 0, .src = src, .list = head};

    char c;
    while ((c = read_next(&acc)))
    {
        char peek = read_peek(&acc);

        if (acc.type == Empty)
        {

            if (c == '\n' || c == ' ' || c == '\t')
            {
                reset_acc(&acc);
                continue;
            }
            else if (c == ';')
            {
                make_token(TSemicolon, &acc, 0);
            }
            else if (c == '"')
            {
                acc.type = InString;
            }
            else if (c == '.' && peek == '.')
            {
                make_token(TRest, &acc, 0);
            }
            else if (c == '{')
            {
                make_token(TOpenCurlyBracket, &acc, 0);
            }
            else if (c == '}')
            {
                make_token(TCloseCurlyBracket, &acc, 0);
            }
            else if (c == '[')
            {
                make_token(TOpenSquareBracket, &acc, 0);
            }
            else if (c == ']')
            {
                make_token(TCloseSquareBracket, &acc, 0);
            }
            else if (c == '(')
            {
                make_token(TOpenParens, &acc, 0);
            }
            else if (c == ')')
            {
                make_token(TCloseParens, &acc, 0);
            }
            else if (c == '_')
            {
                make_token(TPlaceholder, &acc, 0);
            }
            else if (c == '#')
            {
                acc.type = InNamespaceIdentifier;
            }
            else if (c == '@')
            {
                acc.type = InEffectIdentifier;
            }
            else if (is_letter(c))
            {
                acc.type = InIdentifier;
            }
            else if (c == ':')
            {
                make_token(TColon, &acc, 0);
            }
            else if (c == '=' && peek == '>')
            {
                read_next(&acc);
                make_token(TFnArrow, &acc, 0);
            }
            else if (c == '=' && peek == '=')
            {
                read_next(&acc);
                make_token(TEq, &acc, 0);
            }
            else if (c == '=')
            {
                make_token(TAssign, &acc, 0);
            }
            else if (c == '|' && peek == '>')
            {
                read_next(&acc);
                make_token(TPipe, &acc, 0);
            }
            else if (c == '|' && peek == '|')
            {
                read_next(&acc);
                make_token(TOr, &acc, 0);
            }
            else if (c == '|')
            {
                make_token(TBitOr, &acc, 0);
            }
            else if (c == '&' && peek == '&')
            {
                read_next(&acc);
                make_token(TAnd, &acc, 0);
            }
            else if (c == '&')
            {
                make_token(TBitAnd, &acc, 0);
            }
            else if (c == '^')
            {
                make_token(TXor, &acc, 0);
            }
            else if (c == '!' && peek == '=')
            {
                read_next(&acc);
                make_token(TNeq, &acc, 0);
            }
            else if (c == '!')
            {
                make_token(TNot, &acc, 0);
            }
            else if (c == '<' && peek == '=')
            {
                read_next(&acc);
                make_token(TSe, &acc, 0);
            }
            else if (c == '<')
            {
                make_token(TSt, &acc, 0);
            }
            else if (c == '>' && peek == '=')
            {
                read_next(&acc);
                make_token(TGe, &acc, 0);
            }
            else if (c == '>')
            {
                make_token(TGt, &acc, 0);
            }
            else if (c == '.')
            {
                make_token(TAccess, &acc, 0);
            }
            else if (c == '-' && peek == '-')
            {
                read_next(&acc);
                make_token(TDecrement, &acc, 0);
            }
            else if (c == '+' && peek == '+')
            {
                read_next(&acc);
                make_token(TIncrement, &acc, 0);
            }
            else if (c == '+')
            {
                make_token(TAdd, &acc, 0);
            }
            else if (c == '*')
            {
                make_token(TMul, &acc, 0);
            }
            else if (c == '-')
            {
                make_token(TSub, &acc, 0);
            }
            else if (is_number(c))
            {
                acc.type = InNumber;
            }
            else if (c == '/' && peek == '/')
            {
                acc.type = InComment;
            }
            else if (c == '/')
            {
                make_token(TDiv, &acc, 0);
            }
            else if (c == ',')
            {
                make_token(TComma, &acc, 0);
            }
            else
            {
                make_token(TInvalid, &acc, 0);
                acc.type = Empty;
            }
        }

        if (acc.type == InNumber && !is_number(peek))
        {
            if (peek == 'u')
            {
                acc.type = InUint;
            }
            else if (peek == 'i')
            {
                acc.type = InInt;
            }
            else if (peek == 'f')
            {
                acc.type = InFloat;
            }
            else if (peek == '.')
            {
                acc.type = InDecimal;
            }
            else if (peek == 'f')
            {
                acc.type = InFloat;
            }
            else
            {
                make_token(TNumber, &acc, 0);
            }
        }
        else if (acc.type == InDecimal && peek == 'f')
        {
            acc.type = InFloat;
        }
        else if (acc.type == InDecimal && !is_number(peek))
        {
            make_token(TDecimal, &acc, 0);
        }
        else if (acc.type == InFloat)
        {
            // Float32
            if (c == '3' && peek == '2')
            {
                read_next(&acc);
                make_token(TFloat32, &acc, 3);
            }
            // Float64
            else if (c == '6' && peek == '4')
            {
                read_next(&acc);
                make_token(TFloat64, &acc, 3);
            }
        }
        else if (acc.type == InUint)
        {
            // Uint8
            if (c == '8')
            {
                make_token(TUint8, &acc, 2);
            }
            // Uint16
            else if (c == '1' && peek == '6')
            {
                read_next(&acc);
                make_token(TUint16, &acc, 3);
            }
            // Uint32
            else if (c == '3' && peek == '2')
            {
                read_next(&acc);
                make_token(TUint32, &acc, 3);
            }
            // Uint64
            else if (c == '6' && peek == '4')
            {
                read_next(&acc);
                make_token(TUint64, &acc, 3);
            }
        }
        else if (acc.type == InInt)
        {
            // Int8
            if (c == '8')
            {
                make_token(TInt8, &acc, 2);
            }
            // Int16
            else if (c == '1' && peek == '6')
            {
                read_next(&acc);
                make_token(TInt16, &acc, 3);
            }
            // Int32
            else if (c == '3' && peek == '2')
            {
                read_next(&acc);
                make_token(TInt32, &acc, 3);
            }
            // Int64
            else if (c == '6' && peek == '4')
            {
                read_next(&acc);
                make_token(TInt64, &acc, 3);
            }
        }
        else if (acc.type == InComment && is_lineend(peek))
        {
            make_token(TComment, &acc, 0);
        }
        else if (acc.type == InIdentifier && !is_valid_identifier_part(peek))
        {
            if (is_prefix(acc.content, "true"))
            {
                make_token(TTrue, &acc, 0);
            }
            else if (is_prefix(acc.content, "else"))
            {
                make_token(TElse, &acc, 0);
            }
            else if (is_prefix(acc.content, "break"))
            {
                make_token(TBreak, &acc, 0);
            }
            else if (is_prefix(acc.content, "while"))
            {
                make_token(TWhile, &acc, 0);
            }
            else if (is_prefix(acc.content, "continue"))
            {
                make_token(TContinue, &acc, 0);
            }
            else if (is_prefix(acc.content, "for"))
            {
                make_token(TFor, &acc, 0);
            }
            else if (is_prefix(acc.content, "fn"))
            {
                make_token(TFn, &acc, 0);
            }
            else if (is_prefix(acc.content, "if"))
            {
                make_token(TIf, &acc, 0);
            }
            else if (is_prefix(acc.content, "false"))
            {
                make_token(TFalse, &acc, 0);
            }
            else if (is_prefix(acc.content, "struct"))
            {
                make_token(TStruct, &acc, 0);
            }
            else if (is_prefix(acc.content, "union"))
            {
                make_token(TUnion, &acc, 0);
            }
            else if (is_prefix(acc.content, "return"))
            {
                make_token(TReturn, &acc, 0);
            }
            else if (is_prefix(acc.content, "resume"))
            {
                make_token(TReturn, &acc, 0);
            }
            else if (is_prefix(acc.content, "with"))
            {
                make_token(TWith, &acc, 0);
            }
            else if (is_prefix(acc.content, "const"))
            {
                make_token(TConst, &acc, 0);
            }
            else if (is_prefix(acc.content, "namespace"))
            {
                make_token(TNamespace, &acc, 0);
            }
            else if (is_prefix(acc.content, "var"))
            {
                make_token(TVar, &acc, 0);
            }
            else if (is_prefix(acc.content, "type"))
            {
                make_token(TType, &acc, 0);
            }
            else if (is_prefix(acc.content, "match"))
            {
                make_token(TMatch, &acc, 0);
            }
            else
            {
                make_token(TIdentifier, &acc, 0);
            }
        }
        else if (acc.type == InNamespaceIdentifier && !is_valid_identifier_part(peek))
        {
            make_token(TNamespaceIdentifier, &acc, 0);
        }
        else if (acc.type == InEffectIdentifier && !is_valid_identifier_part(peek))
        {
            make_token(TEffectIdentifier, &acc, 0);
        }
        else if (acc.type == InString && peek == '{')
        {
            make_token(TStringInterpolation, &acc, 0);
            read_next(&acc);
            reset_acc(&acc);
            acc.type = InStringInterpolatedValue;
        }
        else if (acc.type == InStringInterpolation && peek == '{')
        {
            make_token(TStringInterpolation, &acc, 0);
            read_next(&acc);
            reset_acc(&acc);
            acc.type = InStringInterpolatedValue;
        }
        else if (acc.type == InStringInterpolatedValue && c == '}')
        {
            make_token(TIdentifier, &acc, 1);
            acc.type = InStringInterpolation;

            if (peek == '{')
            {
                read_next(&acc);
                reset_acc(&acc);
                acc.type = InStringInterpolatedValue;
            }
        }
        else if (acc.type == InStringInterpolation && c == '"')
        {
            make_token(TStringInterpolation, &acc, 0);
        }
        else if (acc.type == InString && peek == '"')
        {
            read_next(&acc);
            make_token(TString, &acc, 0);
        }
    }

    make_token(TEnd, &acc, 0);

    if (acc.len > 0 && acc.type != Empty)
    {
        printf("Unhandled state: %s\n with leftover content %s", get_acc_type(acc.type), acc.content);
    }

    return head;
}

void print_tokens(char *src)
{
    Token *token = tokenize(src);
    while (token != NULL)
    {
        printf("%s(%s)\n", get_token_name(token->type), token->content);
        token = token->next;
    }
}
