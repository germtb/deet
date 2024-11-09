#ifndef C_PRINT_H_
#define C_PRINT_H_

#include "../zone/zone.h"
#include "../array/array.h"

typedef enum ParentType
{
    InStructDeclaration,
    InGlobalScope,
    InVarDeclaration,
} ParentType;

typedef struct Context
{
    struct Context *parent;
    struct Context *child;
    struct Hashmap *types;
    struct Hashmap *variables;
    int depth;
    char *parent_name;
    ParentType parent_type;
    Zone *zone;
    Array *output;
} Context;

#endif // C_PRINT_H_
