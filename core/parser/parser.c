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
    Node *node = make_node(FunctionArg);

    append_child(node, identifier(acc));
    if (match(acc, TColon))
    {
        append_child(node, type_expression(acc));
    }

    return node;
}

static Node *function_args(ParserAcc *acc)
{
    Node *node = make_node(FunctionArgs);

    if (match(acc, TOpenParens))
    {
        while (!match(acc, TCloseParens))
        {
            append_child(node, function_arg(acc));
        }
    }
    else
    {
        Node *arg = make_node(FunctionArg);
        append_child(arg, identifier(acc));
        append_child(node, arg);
    }

    return node;
}

static Node *fn(ParserAcc *acc)
{
    Node *node = make_node(Function);

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
    Node *node = make_node(FunctionDeclaration);

    append_child(node, identifier(acc));
    append_child(node, function_args(acc));

    if (match(acc, TColon))
    {
        append_child(node, type_expression(acc));
        append_child(node, block(acc));
    }
    else
    {
        Node *void_type = make_node(TypeIdentifier);
        void_type->str_value = "void";
        append_child(node, void_type);
        append_child(node, block(acc));
    }

    return node;
}

static Node *array(ParserAcc *acc)
{
    Node *node = make_node(Array);

    while (!match(acc, TCloseSquareBracket))
    {
        append_child(node, expression(acc));
    }

    return node;
}

static Node *struct_property(ParserAcc *acc)
{
    Node *node = make_node(StructProperty);

    append_child(node, identifier(acc));
    consume(acc, TColon);
    append_child(node, expression(acc));

    return node;
}

static Node *struct_expression(ParserAcc *acc)
{
    Node *node = make_node(Struct);

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
    Node *node = make_node(Block);

    consume(acc, TOpenCurlyBracket);

    while (!match(acc, TCloseCurlyBracket))
    {
        append_child(node, statement(acc));
    }

    return node;
}

static Node *else_if_node(ParserAcc *acc)
{
    Node *node = make_node(ElseIf);

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

    Node *node = make_node(Else);
    append_child(node, block(acc));

    return node;
}

static Node *if_node(ParserAcc *acc)
{
    Node *node = make_node(If);

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
        Node *node = make_node(True);
        return node;
    }
    else if (match(acc, TFalse))
    {
        Node *node = make_node(False);
        return node;
    }
    else if (match(acc, TString))
    {
        Node *node = make_node(StringLiteral);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TStringInterpolation))
    {
        Node *str = make_node(StringInterpolation);
        str->str_value = previous(acc)->content;

        while (true)
        {

            if (match(acc, TIdentifier))
            {
                Node *node = make_node(Identifier);
                node->str_value = previous(acc)->content;
                append_child(str, node);
            }
            else if (match(acc, TStringInterpolation))
            {
                Node *node = make_node(StringInterpolation);
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
        Node *node = make_node(Uint8);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TUint16))
    {
        Node *node = make_node(Uint16);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TUint32))
    {
        Node *node = make_node(Uint32);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TUint64))
    {
        Node *node = make_node(Uint64);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TInt8))
    {
        Node *node = make_node(Int8);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TInt16))
    {
        Node *node = make_node(Int16);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TInt32))
    {
        Node *node = make_node(Int32);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TInt64))
    {
        Node *node = make_node(Int64);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TFloat32))
    {
        Node *node = make_node(Float32);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TFloat64))
    {
        Node *node = make_node(Float64);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TNumber))
    {
        Node *node = make_node(Number);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TDecimal))
    {
        Node *node = make_node(Decimal);
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
    Node *node = make_node(CallArg);

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
    Node *node = make_node(CallArgs);

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
        Node *node = make_node(Pipe);
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
        Node *node = make_node(Call);
        append_child(node, left);
        append_child(node, call_args(acc));
        return node;
    }

    return left;
}

static Node *post_increment(ParserAcc *acc)
{
    Node *node = make_node(PostIncrement);
    append_child(node, identifier(acc));
    return node;
}

static Node *post_decrement(ParserAcc *acc)
{
    Node *node = make_node(PostDecrement);
    append_child(node, identifier(acc));
    return node;
}

static Node *pre_increment(ParserAcc *acc)
{
    Node *node = make_node(PreIncrement);
    append_child(node, identifier(acc));
    return node;
}

static Node *pre_decrement(ParserAcc *acc)
{
    Node *node = make_node(PreDecrement);
    append_child(node, identifier(acc));
    return node;
}

static Node *unary(ParserAcc *acc)
{
    if (match(acc, TNot))
    {
        Node *node = make_node(Not);
        append_child(node, call_expression(acc));
        return node;
    }
    else if (match(acc, TSub))
    {
        Node *node = make_node(Negative);
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
        Node *node = make_node(Mul);
        append_child(node, left);
        append_child(node, unary(acc));
        return node;
    }
    else if (match(acc, TDiv))
    {
        Node *node = make_node(Div);
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
        Node *node = make_node(Add);
        append_child(node, left);
        append_child(node, factor(acc));
        return node;
    }
    else if (match(acc, TSub))
    {
        Node *node = make_node(Sub);
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
        Node *node = make_node(St);
        append_child(node, left);
        append_child(node, term(acc));
        return node;
    }
    else if (match(acc, TSe))
    {
        Node *node = make_node(Se);
        append_child(node, left);
        append_child(node, term(acc));
        return node;
    }
    else if (match(acc, TGt))
    {
        Node *node = make_node(Gt);
        append_child(node, left);
        append_child(node, term(acc));
        return node;
    }
    else if (match(acc, TGe))
    {
        Node *node = make_node(Ge);
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
        Node *node = make_node(Eq);
        append_child(node, left);
        append_child(node, comparison(acc));
        return node;
    }
    else if (match(acc, TNeq))
    {
        Node *node = make_node(Neq);
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
        Node *node = make_node(And);
        append_child(node, left);
        append_child(node, expression(acc));
        return node;
    }
    else if (match(acc, TOr))
    {
        Node *node = make_node(Or);
        append_child(node, left);
        append_child(node, expression(acc));
        return node;
    }
    else if (match(acc, TXor))
    {
        Node *node = make_node(Xor);
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
        Node *node = make_node(BitAnd);
        append_child(node, left);
        append_child(node, boolean_operation(acc));
        return node;
    }
    else if (match(acc, TBitOr))
    {
        Node *node = make_node(BitOr);
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
        Node *node = make_node(Access);
        append_child(node, left);
        append_child(node, identifier(acc));

        if (match(acc, TOpenParens))
        {
            Node *call = make_node(Call);
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
    Node *node = make_node(Return);
    append_child(node, expression(acc));
    return node;
}

static Node *for_statement(ParserAcc *acc)
{
    Node *node = make_node(For);

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
    Node *node = make_node(While);

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
        Node *node = make_node(Break);
        consume(acc, TSemicolon);
        return node;
    }
    else if (match(acc, TContinue))
    {
        Node *node = make_node(Continue);
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
        Node *node = make_node(Comment);
        node->str_value = previous(acc)->content;
        return node;
    }
    else if (match(acc, TIf))
    {
        return if_node(acc);
    }
    else
    {
        Node *node = make_node(Statement);
        append_child(node, expression(acc));
        consume(acc, TSemicolon);
        return node;
    }
}

static Node *struct_type_property(ParserAcc *acc)
{
    Node *node = make_node(StructProperty);

    append_child(node, identifier(acc));
    consume(acc, TColon);
    append_child(node, type_expression(acc));

    return node;
}

static Node *struct_type(ParserAcc *acc)
{
    Node *node = make_node(Struct);

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
    Node *node = make_node(ConstDeclaration);

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
    Node *node = make_node(Reassignment);

    append_child(node, identifier(acc));
    consume(acc, TAssign);
    append_child(node, expression(acc));

    return node;
}

static Node *var_declaration(ParserAcc *acc)
{
    Node *node = make_node(VarDeclaration);
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
    Node *node = make_node(StructDeclaration);

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
    Node *node = make_node(TypeDeclaration);

    append_child(node, type_identifier(acc));
    consume(acc, TAssign);
    append_child(node, type_expression(acc));

    return node;
}

static Node *effect_identifier(ParserAcc *acc)
{

    if (match(acc, TEffectIdentifier))
    {
        Node *node = make_node(EffectIdentifier);
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
        Node *node = make_node(Identifier);
        node->str_value = previous(acc)->content;

        if (match(acc, TIncrement))
        {
            Node *increment = make_node(PostIncrement);
            append_child(increment, node);
            return increment;
        }
        else if (match(acc, TDecrement))
        {
            Node *decrement = make_node(PostDecrement);
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
        Node *node = make_node(TypeIdentifier);
        node->str_value = previous(acc)->content;

        if (match(acc, TOpenSquareBracket))
        {
            Node *array = make_node(Array);
            append_child(array, node);
            consume(acc, TCloseSquareBracket);
            return array;
        }
        else if (match(acc, TSt))
        {
            Node *type_parameters = make_node(TypeParameters);

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
    Node *node = make_node(Program);

    while (!match(acc, TEnd))
    {
        append_child(node, statement(acc));
    }

    append_child(node, make_node(End));

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
    case Number:
        return "Number";
    case Decimal:
        return "Decimal";
    case Int8:
        return "Int8";
    case Int16:
        return "Int16";
    case Int32:
        return "Int32";
    case Int64:
        return "Int64";
    case Uint8:
        return "Uint8";
    case Uint16:
        return "Uint16";
    case Uint32:
        return "Uint32";
    case Uint64:
        return "Uint64";
    case Float32:
        return "Float32";
    case Float64:
        return "Float64";
    case StringLiteral:
        return "StringLiteral";
    case StringInterpolation:
        return "StringInterpolation";
    case True:
        return "True";
    case False:
        return "False";
    case Struct:
        return "Struct";
    case StructProperty:
        return "StructProperty";
    case Access:
        return "Access";
    case Array:
        return "Array";
    case If:
        return "If";
    case Else:
        return "Else";
    case ElseIf:
        return "ElseIf";
    case While:
        return "While";
    case For:
        return "For";
    case Continue:
        return "Continue";
    case Break:
        return "Break";
    case StructDeclaration:
        return "StructDeclaration";
    case TypeParameters:
        return "TypeParameters";
    case Match:
        return "Match";
    case MatchCase:
        return "MatchCase";
    case DefaultCase:
        return "DefaultCase";
    case Pattern:
        return "Pattern";
    case Return:
        return "Return";
    case Identifier:
        return "Identifier";
    case EffectIdentifier:
        return "EffectIdentifier";
    case FunctionDeclaration:
        return "FunctionDeclaration";
    case Function:
        return "Function";
    case FunctionArgs:
        return "FunctionArgs";
    case FunctionArg:
        return "FunctionArg";
    case Call:
        return "Call";
    case Pipe:
        return "Pipe";
    case CallArg:
        return "CallArg";
    case CallArgs:
        return "CallArgs";
    case Add:
        return "Add";
    case Sub:
        return "Sub";
    case Mul:
        return "Mul";
    case Div:
        return "Div";
    case Ge:
        return "Ge";
    case Gt:
        return "Gt";
    case Se:
        return "Se";
    case St:
        return "St";
    case Eq:
        return "Eq";
    case Neq:
        return "Neq";
    case And:
        return "And";
    case Or:
        return "Or";
    case Xor:
        return "Xor";
    case BitAnd:
        return "BitAnd";
    case BitOr:
        return "BitOr";
    case Not:
        return "Not";
    case Negative:
        return "Negative";
    case PostIncrement:
        return "PostIncrement";
    case PostDecrement:
        return "PostDecrement";
    case PreIncrement:
        return "PreIncrement";
    case PreDecrement:
        return "PreDecrement";
    case Reassignment:
        return "Reassignment";
    case TypeDeclaration:
        return "TypeDeclaration";
    case VarDeclaration:
        return "VarDeclaration";
    case ConstDeclaration:
        return "ConstDeclaration";
    case Statement:
        return "Statement";
    case Comment:
        return "Comment";
    case Program:
        return "Program";
    case TypeIdentifier:
        return "TypeIdentifier";
    case Block:
        return "Block";
    case End:
        return "End";
    default:
    {
        printf("Unknown node type: %d\n", type);
        return "Unknown";
    }
    }
}
