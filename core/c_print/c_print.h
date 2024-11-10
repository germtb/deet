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

typedef struct Parent
{
    ParentType type;
    char *name;
} Parent;

typedef struct Context
{
    struct Context *child;
    struct Hashmap *types;
    struct Hashmap *variables;
    int depth;
    Parent parent;
    Zone *zone;
    Array *output;
} Context;

char *c_print(char *src, bool add_headers);

#endif // C_PRINT_H_
