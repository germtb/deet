#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>

enum NodeType
{
    // Number
    NNumber,
    NDecimal,
    NInt8,
    NInt16,
    NInt32,
    NInt64,
    NUint8,
    NUint16,
    NUint32,
    NUint64,
    NFloat32,
    NFloat64,

    // Strings
    NStringLiteral,
    NStringInterpolation,

    // Booleans
    NTrue,
    NFalse,

    // Structs
    NStruct,
    NStructType,
    NStructProperty,
    NAccess,

    // Blocks
    NBlock,

    // Arrays
    NArray,

    // Flow
    NIf,
    NElse,
    NElseIf,
    NMatch,
    NMatchCase,
    NDefaultCase,
    NPattern,

    // Zones
    NReturn,

    // Identifiers
    NIdentifier,
    NTypeIdentifier,
    NEffectIdentifier,

    // Effects
    NEffect,

    // Functions
    NFunctionDeclaration,
    NFunction,
    NFunctionArgs,
    NFunctionArg,
    NCall,
    NCallArgs,
    NCallArg,
    NPipe,

    // Binary
    NAdd,
    NSub,
    NMul,
    NDiv,
    NGe,
    NGt,
    NSe,
    NSt,
    NEq,
    NNeq,
    NAnd,
    NOr,
    NXor,
    NBitAnd,
    NBitOr,

    // Unary
    NNot,
    NNegative,
    NPostIncrement,
    NPostDecrement,
    NPreIncrement,
    NPreDecrement,

    // Statements
    NReassignment,
    NConstDeclaration,
    NVarDeclaration,
    NTypeDeclaration,
    NStatement,
    NComment,

    // Program
    NProgram,
    NEnd,

    // Types
    NStructDeclaration,
    NTypeParameters,

    // Loops
    NFor,
    NWhile,
    NContinue,
    NBreak,
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
