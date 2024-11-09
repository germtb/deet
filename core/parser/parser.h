#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>

enum NodeType
{
    // Number
    Number,
    Decimal,
    Int8,
    Int16,
    Int32,
    Int64,
    Uint8,
    Uint16,
    Uint32,
    Uint64,
    Float32,
    Float64,

    // Strings
    StringLiteral,
    StringInterpolation,

    // Booleans
    True,
    False,

    // Structs
    Struct,
    StructProperty,
    Access,

    // Blocks
    Block,

    // Arrays
    Array,

    // Flow
    If,
    Else,
    ElseIf,
    Match,
    MatchCase,
    DefaultCase,
    Pattern,

    // Zones
    Return,

    // Identifiers
    Identifier,
    TypeIdentifier,
    EffectIdentifier,

    // Functions
    FunctionDeclaration,
    Function,
    FunctionArgs,
    FunctionArg,
    Call,
    CallArgs,
    CallArg,

    // Binary
    Add,
    Sub,
    Mul,
    Div,
    Ge,
    Gt,
    Se,
    St,
    Eq,
    Neq,
    And,
    Or,
    Xor,
    BitAnd,
    BitOr,

    // Unary
    Not,
    Negative,
    PostIncrement,
    PostDecrement,
    PreIncrement,
    PreDecrement,

    // Statements
    Reassignment,
    Declaration,
    TypeDeclaration,
    Statement,
    Comment,

    // Program
    Program,
    End,

    // Types
    StructDeclaration,
};

const char *get_node_name(enum NodeType type);

typedef struct Node
{
    enum NodeType type;
    char *str_value;
    struct NodeList *head;
    struct NodeList *tail;
} Node;

typedef struct NodeList
{
    struct Node *value;
    struct NodeList *next;
} NodeList;

Node *parse(char *src);
int children_count(Node *node);

#endif // PARSER_H_
