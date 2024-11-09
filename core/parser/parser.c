#include "../lexer/lexer.h"
#include "./parser.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

typedef struct ParserAcc
{
    Token *head;
    Token *last;
} ParserAcc;

static Node *access(ParserAcc *acc);
static Node *array(ParserAcc *acc);
static Node *bit_operation(ParserAcc *acc);
static Node *boolean_operation(ParserAcc *acc);
static Node *call_arg(ParserAcc *acc);
static Node *call_expression(ParserAcc *acc);
static Node *comparison(ParserAcc *acc);
static Node *const_declaration(ParserAcc *acc);
static Node *else_if_node(ParserAcc *acc);
static Node *else_node(ParserAcc *acc);
static Node *else_node(ParserAcc *acc);
static Node *equality(ParserAcc *acc);
static Node *expression(ParserAcc *acc);
static Node *factor(ParserAcc *acc);
static Node *fn_declaration(ParserAcc *acc);
static Node *fn(ParserAcc *acc);
static Node *function_arg(ParserAcc *acc);
static Node *identifier(ParserAcc *acc);
static Node *identifier(ParserAcc *acc);
static Node *if_node(ParserAcc *acc);
static Node *make_node(enum NodeType type);
static Node *post_decrement(ParserAcc *acc);
static Node *post_increment(ParserAcc *acc);
static Node *pre_decrement(ParserAcc *acc);
static Node *pre_increment(ParserAcc *acc);
static Node *primary(ParserAcc *acc);
static Node *program(ParserAcc *acc);
static Node *return_statement(ParserAcc *acc);
static Node *statement(ParserAcc *acc);
static Node *struct_declaration(ParserAcc *acc);
static Node *struct_expression(ParserAcc *acc);
static Node *struct_property(ParserAcc *acc);
static Node *struct_type_property(ParserAcc *acc);
static Node *struct_type(ParserAcc *acc);
static Node *term(ParserAcc *acc);
static Node *type_declaration(ParserAcc *acc);
static Node *type_expression(ParserAcc *acc);
static Node *type_identifier(ParserAcc *acc);
static Node *unary(ParserAcc *acc);
static Node *var_declaration(ParserAcc *acc);
static Node *var_reassign(ParserAcc *acc);
static Node *effect_identifier(ParserAcc *acc);
static Node *block(ParserAcc *acc);

static Node *make_node(enum NodeType type)
{
    // printf("Making node of type %s\n", get_node_name(type));
    Node *node = malloc(sizeof(Node));
    node->type = type;
    node->head = NULL;
    node->tail = NULL;
    return node;
}

static void append_child(Node *parent, Node *child)
{
    NodeList *list = malloc(sizeof(NodeList));
    list->value = child;
    list->next = NULL;

    if (parent->head == NULL)
    {
        parent->head = list;
        parent->tail = list;
    }
    else
    {
        parent->tail->next = list;
        parent->tail = list;
    }
}

static Token *current_token(ParserAcc *acc)
{
    return acc->head;
}

static bool peek(ParserAcc *acc, enum TokenType expected)
{
    return acc->head->type == expected;
}

static bool peek_n(ParserAcc *acc, enum TokenType expected, int n)
{
    Token *head = acc->head;

    for (int i = 0; i < n; i++)
    {
        head = head->next;
    }

    return head->type == expected;
}

static void next(ParserAcc *acc)
{
    acc->last = acc->head;
    acc->head = acc->head->next;
}

static Token *previous(ParserAcc *acc)
{
    return acc->last;
}

static bool match(ParserAcc *acc, enum TokenType expected)
{
    if (current_token(acc)->type == expected)
    {
        next(acc);
        return true;
    }

    return false;
}

static void consume(ParserAcc *acc, enum TokenType expected)
{
    if (!match(acc, expected))
    {
        printf("Expected to consume %s, got %s\n", get_token_name(expected), get_token_name(current_token(acc)->type));
        assert(false);
    }
}

static Node *function_arg(ParserAcc *acc)
{
    Node *node = make_node(NFunctionArg);

    append_child(node, identifier(acc));
    if (match(acc, TColon))
    {
        append_child(node, type_expression(acc));
    }

    return node;
}

static Node *function_args(ParserAcc *acc)
{
    Node *node = make_node(NFunctionArgs);

    if (match(acc, TOpenParens))
    {
        while (!match(acc, TCloseParens))
        {
            append_child(node, function_arg(acc));
        }
    }
    else
    {
        Node *arg = make_node(NFunctionArg);
        append_child(arg, identifier(acc));
        append_child(node, arg);
    }

    return node;
}

static Node *fn(ParserAcc *acc)
{
    Node *node = make_node(NFunction);

    append_child(node, function_args(acc));

    if (match(acc, TColon))
    {
        append_child(node, type_expression(acc));
    }

    consume(acc, TFnArrow);

    if (peek(acc, TOpenCurlyBracket))
    {
        append_child(node, block(acc));
    }
    else
    {
        append_child(node, expression(acc));
    }

    return node;
}

static Node *fn_declaration(ParserAcc *acc)
{
    Node *node = make_node(NFunctionDeclaration);

    append_child(node, identifier(acc));
    append_child(node, function_args(acc));

    if (match(acc, TColon))
    {
        append_child(node, type_expression(acc));
        append_child(node, block(acc));
    }
    else
    {
        Node *void_type = make_node(NTypeIdentifier);
        void_type->str_value = "void";
        append_child(node, void_type);
        append_child(node, block(acc));
    }

    return node;
}

static Node *array(ParserAcc *acc)
{
    Node *node = make_node(NArray);

    while (!match(acc, TCloseSquareBracket))
    {
        append_child(node, expression(acc));
    }

    return node;
}

static Node *struct_property(ParserAcc *acc)
{
    Node *node = make_node(NStructProperty);

    append_child(node, identifier(acc));
    consume(acc, TColon);
    append_child(node, expression(acc));

    return node;
}

static Node *struct_expression(ParserAcc *acc)
{
    Node *node = make_node(NStruct);

    if (match(acc, TCloseCurlyBracket))
    {
        return node;
    }

    while (true)
    {
        append_child(node, struct_property(acc));

        if (!match(acc, TComma))
        {
            break;
        }
    }

    consume(acc, TCloseCurlyBracket);

    return node;
}

static Node *block(ParserAcc *acc)
{
    Node *node = make_node(NBlock);

    consume(acc, TOpenCurlyBracket);

    while (!match(acc, TCloseCurlyBracket))
    {
        append_child(node, statement(acc));
    }

    return node;
}

static Node *else_if_node(ParserAcc *acc)
{
    Node *node = make_node(NElseIf);

    consume(acc, TOpenParens);
    append_child(node, expression(acc));
    consume(acc, TCloseParens);
    append_child(node, block(acc));

    if (match(acc, TElse))
    {
        append_child(node, else_node(acc));
    }

    return node;
}

static Node *else_node(ParserAcc *acc)
{
    if (match(acc, TIf))
    {
        return else_if_node(acc);
    }

    Node *node = make_node(NElse);
    append_child(node, block(acc));

    return node;
}

static Node *if_node(ParserAcc *acc)
{
    Node *node = make_node(NIf);

    consume(acc, TOpenParens);
    append_child(node, expression(acc));
    consume(acc, TCloseParens);
    append_child(node, block(acc));

    if (match(acc, TElse))
    {
        append_child(node, else_node(acc));
    }

    return node;
}

static Node *primary(ParserAcc *acc)
{
    if (match(acc, TTrue))
    {
        Node *node = make_node(NTrue);
        return node;
    }
    else if (match(acc, TFalse))
    {
        Node *node = make_node(NFalse);
        return node;
    }
    else if (match(acc, TString))
    {
        Node *node = make_node(NStringLiteral);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TStringInterpolation))
    {
        Node *str = make_node(NStringInterpolation);
        str->str_value = previous(acc)->content;

        while (true)
        {

            if (match(acc, TIdentifier))
            {
                Node *node = make_node(NIdentifier);
                node->str_value = previous(acc)->content;
                append_child(str, node);
            }
            else if (match(acc, TStringInterpolation))
            {
                Node *node = make_node(NStringInterpolation);
                node->str_value = previous(acc)->content;
                append_child(str, node);
            }
            else
            {
                break;
            }
        }

        return str;
    }
    else if (match(acc, TUint8))
    {
        Node *node = make_node(NUint8);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TUint16))
    {
        Node *node = make_node(NUint16);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TUint32))
    {
        Node *node = make_node(NUint32);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TUint64))
    {
        Node *node = make_node(NUint64);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TInt8))
    {
        Node *node = make_node(NInt8);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TInt16))
    {
        Node *node = make_node(NInt16);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TInt32))
    {
        Node *node = make_node(NInt32);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TInt64))
    {
        Node *node = make_node(NInt64);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TFloat32))
    {
        Node *node = make_node(NFloat32);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TFloat64))
    {
        Node *node = make_node(NFloat64);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TNumber))
    {
        Node *node = make_node(NNumber);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TDecimal))
    {
        Node *node = make_node(NDecimal);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TOpenCurlyBracket))
    {
        return struct_expression(acc);
    }
    else if (match(acc, TOpenSquareBracket))
    {
        return array(acc);
    }
    else if (match(acc, TFn))
    {
        return fn(acc);
    }
    else if (peek(acc, TIdentifier))
    {
        return identifier(acc);
    }
    else if (match(acc, TOpenParens))
    {
        Node *group = expression(acc);
        consume(acc, TCloseParens);
        return group;
    }
    else
    {
        printf("Expected a primary expression, got %s. The previous token was %s\n", get_token_name(current_token(acc)->type), get_token_name(previous(acc)->type));
        assert(false);
    }
}

static Node *call_arg(ParserAcc *acc)
{
    Node *node = make_node(NCallArg);

    if (peek(acc, TIdentifier) || peek(acc, TIdentifier))
    {
        append_child(node, identifier(acc));
        consume(acc, TColon);
    }

    append_child(node, expression(acc));

    return node;
}

static Node *call_args(ParserAcc *acc)
{
    Node *node = make_node(NCallArgs);

    while (!match(acc, TCloseParens))
    {
        append_child(node, call_arg(acc));
    }

    return node;
}

static Node *pipe_expression(ParserAcc *acc)
{
    Node *left = primary(acc);

    if (peek(acc, TPipe))
    {
        Node *node = make_node(NPipe);
        append_child(node, left);

        while (match(acc, TPipe))
        {
            append_child(node, primary(acc));
        }

        return node;
    }

    return left;
}

static Node *call_expression(ParserAcc *acc)
{
    Node *left = pipe_expression(acc);

    if (match(acc, TOpenParens))
    {
        Node *node = make_node(NCall);
        append_child(node, left);
        append_child(node, call_args(acc));
        return node;
    }

    return left;
}

static Node *post_increment(ParserAcc *acc)
{
    Node *node = make_node(NPostIncrement);
    append_child(node, identifier(acc));
    return node;
}

static Node *post_decrement(ParserAcc *acc)
{
    Node *node = make_node(NPostDecrement);
    append_child(node, identifier(acc));
    return node;
}

static Node *pre_increment(ParserAcc *acc)
{
    Node *node = make_node(NPreIncrement);
    append_child(node, identifier(acc));
    return node;
}

static Node *pre_decrement(ParserAcc *acc)
{
    Node *node = make_node(NPreDecrement);
    append_child(node, identifier(acc));
    return node;
}

static Node *unary(ParserAcc *acc)
{
    if (match(acc, TNot))
    {
        Node *node = make_node(NNot);
        append_child(node, call_expression(acc));
        return node;
    }
    else if (match(acc, TSub))
    {
        Node *node = make_node(NNegative);
        append_child(node, call_expression(acc));
        return node;
    }
    else if (match(acc, TAdd))
    {
        return call_expression(acc);
    }
    else if (match(acc, TIncrement))
    {
        return pre_increment(acc);
    }
    else if (match(acc, TDecrement))
    {
        return pre_decrement(acc);
    }

    return call_expression(acc);
}

static Node *factor(ParserAcc *acc)
{
    Node *left = unary(acc);

    if (match(acc, TMul))
    {
        Node *node = make_node(NMul);
        append_child(node, left);
        append_child(node, unary(acc));
        return node;
    }
    else if (match(acc, TDiv))
    {
        Node *node = make_node(NDiv);
        append_child(node, left);
        append_child(node, unary(acc));
        return node;
    }

    return left;
}

static Node *term(ParserAcc *acc)
{
    Node *left = factor(acc);

    if (match(acc, TAdd))
    {
        Node *node = make_node(NAdd);
        append_child(node, left);
        append_child(node, factor(acc));
        return node;
    }
    else if (match(acc, TSub))
    {
        Node *node = make_node(NSub);
        append_child(node, left);
        append_child(node, factor(acc));
        return node;
    }

    return left;
}

static Node *comparison(ParserAcc *acc)
{
    Node *left = term(acc);

    if (match(acc, TSt))
    {
        Node *node = make_node(NSt);
        append_child(node, left);
        append_child(node, term(acc));
        return node;
    }
    else if (match(acc, TSe))
    {
        Node *node = make_node(NSe);
        append_child(node, left);
        append_child(node, term(acc));
        return node;
    }
    else if (match(acc, TGt))
    {
        Node *node = make_node(NGt);
        append_child(node, left);
        append_child(node, term(acc));
        return node;
    }
    else if (match(acc, TGe))
    {
        Node *node = make_node(NGe);
        append_child(node, left);
        append_child(node, term(acc));
        return node;
    }

    return left;
}

static Node *equality(ParserAcc *acc)
{
    Node *left = comparison(acc);

    if (match(acc, TEq))
    {
        Node *node = make_node(NEq);
        append_child(node, left);
        append_child(node, comparison(acc));
        return node;
    }
    else if (match(acc, TNeq))
    {
        Node *node = make_node(NNeq);
        append_child(node, left);
        append_child(node, comparison(acc));
        return node;
    }

    return left;
}

static Node *boolean_operation(ParserAcc *acc)
{
    Node *left = equality(acc);

    if (match(acc, TAnd))
    {
        Node *node = make_node(NAnd);
        append_child(node, left);
        append_child(node, expression(acc));
        return node;
    }
    else if (match(acc, TOr))
    {
        Node *node = make_node(NOr);
        append_child(node, left);
        append_child(node, expression(acc));
        return node;
    }
    else if (match(acc, TXor))
    {
        Node *node = make_node(NXor);
        append_child(node, left);
        append_child(node, expression(acc));
        return node;
    }

    return left;
}

static Node *bit_operation(ParserAcc *acc)
{
    Node *left = boolean_operation(acc);

    if (match(acc, TBitAnd))
    {
        Node *node = make_node(NBitAnd);
        append_child(node, left);
        append_child(node, boolean_operation(acc));
        return node;
    }
    else if (match(acc, TBitOr))
    {
        Node *node = make_node(NBitOr);
        append_child(node, left);
        append_child(node, boolean_operation(acc));
        return node;
    }

    return left;
}

static Node *access(ParserAcc *acc)
{
    Node *left = bit_operation(acc);

    while (match(acc, TAccess))
    {
        Node *node = make_node(NAccess);
        append_child(node, left);
        append_child(node, identifier(acc));

        if (match(acc, TOpenParens))
        {
            Node *call = make_node(NCall);
            append_child(call, node);
            append_child(call, call_args(acc));
            left = call;
        }
        else
        {
            left = node;
        }
    }

    return left;
}

static Node *expression(ParserAcc *acc)
{
    return access(acc);
}

static Node *return_statement(ParserAcc *acc)
{
    Node *node = make_node(NReturn);
    append_child(node, expression(acc));
    return node;
}

static Node *for_statement(ParserAcc *acc)
{
    Node *node = make_node(NFor);

    consume(acc, TOpenParens);
    consume(acc, TVar);
    append_child(node, var_declaration(acc));
    consume(acc, TSemicolon);
    append_child(node, expression(acc));
    consume(acc, TSemicolon);
    append_child(node, expression(acc));
    consume(acc, TCloseParens);

    append_child(node, block(acc));

    return node;
}

static Node *while_statement(ParserAcc *acc)
{
    Node *node = make_node(NWhile);

    consume(acc, TOpenParens);
    append_child(node, expression(acc));
    consume(acc, TCloseParens);
    append_child(node, block(acc));

    return node;
}

static Node *statement(ParserAcc *acc)
{
    if (match(acc, TConst))
    {
        Node *node = const_declaration(acc);
        consume(acc, TSemicolon);
        return node;
    }
    else if (match(acc, TFor))
    {
        return for_statement(acc);
    }
    else if (match(acc, TWhile))
    {
        return while_statement(acc);
    }
    else if (match(acc, TVar))
    {
        Node *node = var_declaration(acc);
        consume(acc, TSemicolon);
        return node;
    }
    else if (match(acc, TBreak))
    {
        Node *node = make_node(NBreak);
        consume(acc, TSemicolon);
        return node;
    }
    else if (match(acc, TContinue))
    {
        Node *node = make_node(NContinue);
        consume(acc, TSemicolon);
        return node;
    }
    else if (match(acc, TFn))
    {
        return fn_declaration(acc);
    }
    else if (match(acc, TType))
    {
        Node *node = type_declaration(acc);
        consume(acc, TSemicolon);
        return node;
    }
    else if (match(acc, TStruct))
    {
        Node *node = struct_declaration(acc);
        consume(acc, TSemicolon);
        return node;
    }
    else if (match(acc, TReturn))
    {
        Node *node = return_statement(acc);
        consume(acc, TSemicolon);
        return node;
    }
    else if (peek(acc, TIdentifier) && peek_n(acc, TAssign, 1))
    {
        Node *node = var_reassign(acc);
        consume(acc, TSemicolon);
        return node;
    }
    else if (match(acc, TComment))
    {
        Node *node = make_node(NComment);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TIf))
    {
        return if_node(acc);
    }
    else
    {
        Node *node = make_node(NStatement);
        append_child(node, expression(acc));
        consume(acc, TSemicolon);
        return node;
    }
}

static Node *struct_type_property(ParserAcc *acc)
{
    Node *node = make_node(NStructProperty);

    append_child(node, identifier(acc));
    consume(acc, TColon);
    append_child(node, type_expression(acc));

    return node;
}

static Node *struct_type(ParserAcc *acc)
{
    Node *node = make_node(NStruct);

    if (match(acc, TCloseCurlyBracket))
    {
        return node;
    }

    while (true)
    {
        append_child(node, struct_type_property(acc));

        if (!match(acc, TComma))
        {
            break;
        }
    }

    consume(acc, TCloseCurlyBracket);

    return node;
}

static Node *type_expression(ParserAcc *acc)
{
    if (peek(acc, TIdentifier))
    {
        return type_identifier(acc);
    }
    else if (match(acc, TOpenCurlyBracket))
    {
        return struct_type(acc);
    }
    else
    {
        printf("Expected a type expression, got %s\n", get_token_name(current_token(acc)->type));
        assert(false);
    }
}

static Node *const_declaration(ParserAcc *acc)
{
    Node *node = make_node(NConstDeclaration);

    append_child(node, identifier(acc));

    if (match(acc, TColon))
    {
        append_child(node, type_expression(acc));
    }

    consume(acc, TAssign);

    Node *right = expression(acc);
    append_child(node, right);

    return node;
}

static Node *var_reassign(ParserAcc *acc)
{
    Node *node = make_node(NReassignment);

    append_child(node, identifier(acc));
    consume(acc, TAssign);
    append_child(node, expression(acc));

    return node;
}

static Node *var_declaration(ParserAcc *acc)
{
    Node *node = make_node(NVarDeclaration);
    append_child(node, identifier(acc));

    if (match(acc, TColon))
    {
        append_child(node, type_expression(acc));
    }

    consume(acc, TAssign);

    Node *right = expression(acc);
    append_child(node, right);

    return node;
}

static Node *struct_declaration(ParserAcc *acc)
{
    Node *node = make_node(NStructDeclaration);

    append_child(node, type_identifier(acc));
    consume(acc, TOpenCurlyBracket);

    while (!match(acc, TCloseCurlyBracket))
    {
        append_child(node, struct_type_property(acc));
    }

    return node;
}

static Node *type_declaration(ParserAcc *acc)
{
    Node *node = make_node(NTypeDeclaration);

    append_child(node, type_identifier(acc));
    consume(acc, TAssign);
    append_child(node, type_expression(acc));

    return node;
}

static Node *effect_identifier(ParserAcc *acc)
{

    if (match(acc, TEffectIdentifier))
    {
        Node *node = make_node(NEffectIdentifier);
        node->str_value = previous(acc)->content;
        return node;
    }

    printf("Expected a zone identifier, got %s\n", get_token_name(current_token(acc)->type));
    assert(false);
}

static Node *identifier(ParserAcc *acc)
{
    if (match(acc, TIdentifier))
    {
        Node *node = make_node(NIdentifier);
        node->str_value = previous(acc)->content;

        if (match(acc, TIncrement))
        {
            Node *increment = make_node(NPostIncrement);
            append_child(increment, node);
            return increment;
        }
        else if (match(acc, TDecrement))
        {
            Node *decrement = make_node(NPostDecrement);
            append_child(decrement, node);
            return decrement;
        }

        return node;
    }
    else
    {
        printf("Expected an identifier, got %s\n", get_token_name(current_token(acc)->type));
        assert(false);
    }
}

static Node *type_identifier(ParserAcc *acc)
{
    if (match(acc, TIdentifier))
    {
        Node *node = make_node(NTypeIdentifier);
        node->str_value = previous(acc)->content;

        if (match(acc, TOpenSquareBracket))
        {
            Node *array = make_node(NArray);
            append_child(array, node);
            consume(acc, TCloseSquareBracket);
            return array;
        }
        else if (match(acc, TSt))
        {
            Node *type_parameters = make_node(NTypeParameters);

            while (true)
            {
                append_child(type_parameters, type_expression(acc));

                if (!match(acc, TComma))
                {
                    break;
                }
            }

            consume(acc, TGt);

            append_child(node, type_parameters);
        }

        return node;
    }
    else
    {
        printf("Expected an identifier, got %s\n", get_token_name(current_token(acc)->type));
        assert(false);
    }
}

static Node *program(ParserAcc *acc)
{
    Node *node = make_node(NProgram);

    while (!match(acc, TEnd))
    {
        append_child(node, statement(acc));
    }

    append_child(node, make_node(NEnd));

    return node;
}

Node *parse(char *src)
{
    Token *head = tokenize(src);
    ParserAcc acc = {head, NULL};

    next(&acc);
    return program(&acc);
}

int children_count(Node *node)
{
    NodeList *list = node->head;
    int count = 0;

    while (list != NULL)
    {
        list = list->next;
        count++;
    }

    return count;
}

const char *get_node_name(enum NodeType type)
{
    switch (type)
    {
    case NNumber:
        return "Number";
    case NDecimal:
        return "Decimal";
    case NInt8:
        return "Int8";
    case NInt16:
        return "Int16";
    case NInt32:
        return "Int32";
    case NInt64:
        return "Int64";
    case NUint8:
        return "Uint8";
    case NUint16:
        return "Uint16";
    case NUint32:
        return "Uint32";
    case NUint64:
        return "Uint64";
    case NFloat32:
        return "Float32";
    case NFloat64:
        return "Float64";
    case NStringLiteral:
        return "StringLiteral";
    case NStringInterpolation:
        return "StringInterpolation";
    case NTrue:
        return "True";
    case NFalse:
        return "False";
    case NStruct:
        return "Struct";
    case NStructProperty:
        return "StructProperty";
    case NAccess:
        return "Access";
    case NArray:
        return "Array";
    case NIf:
        return "If";
    case NElse:
        return "Else";
    case NElseIf:
        return "ElseIf";
    case NWhile:
        return "While";
    case NFor:
        return "For";
    case NContinue:
        return "Continue";
    case NBreak:
        return "Break";
    case NStructDeclaration:
        return "StructDeclaration";
    case NTypeParameters:
        return "TypeParameters";
    case NMatch:
        return "Match";
    case NMatchCase:
        return "MatchCase";
    case NDefaultCase:
        return "DefaultCase";
    case NPattern:
        return "Pattern";
    case NReturn:
        return "Return";
    case NIdentifier:
        return "Identifier";
    case NEffectIdentifier:
        return "EffectIdentifier";
    case NFunctionDeclaration:
        return "FunctionDeclaration";
    case NFunction:
        return "Function";
    case NFunctionArgs:
        return "FunctionArgs";
    case NFunctionArg:
        return "FunctionArg";
    case NCall:
        return "Call";
    case NPipe:
        return "Pipe";
    case NCallArg:
        return "CallArg";
    case NCallArgs:
        return "CallArgs";
    case NAdd:
        return "Add";
    case NSub:
        return "Sub";
    case NMul:
        return "Mul";
    case NDiv:
        return "Div";
    case NGe:
        return "Ge";
    case NGt:
        return "Gt";
    case NSe:
        return "Se";
    case NSt:
        return "St";
    case NEq:
        return "Eq";
    case NNeq:
        return "Neq";
    case NAnd:
        return "And";
    case NOr:
        return "Or";
    case NXor:
        return "Xor";
    case NBitAnd:
        return "BitAnd";
    case NBitOr:
        return "BitOr";
    case NNot:
        return "Not";
    case NNegative:
        return "Negative";
    case NPostIncrement:
        return "PostIncrement";
    case NPostDecrement:
        return "PostDecrement";
    case NPreIncrement:
        return "PreIncrement";
    case NPreDecrement:
        return "PreDecrement";
    case NReassignment:
        return "Reassignment";
    case NTypeDeclaration:
        return "TypeDeclaration";
    case NVarDeclaration:
        return "VarDeclaration";
    case NConstDeclaration:
        return "ConstDeclaration";
    case NStatement:
        return "Statement";
    case NComment:
        return "Comment";
    case NProgram:
        return "Program";
    case NTypeIdentifier:
        return "TypeIdentifier";
    case NBlock:
        return "Block";
    case NEnd:
        return "End";
    default:
    {
        printf("Unknown node type: %d\n", type);
        return "Unknown";
    }
    }
}
