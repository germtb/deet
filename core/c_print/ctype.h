#ifndef CTYPE_H_
#define CTYPE_H_

#include "c_print.h"
#include "../parser/parser.h"

typedef struct CType
{
    char *name;
    bool is_primitive;
} CType;

CType make_primitive_ctype(char *name);
CType make_pointer_ctype(char *name);
CType make_ctype_not_found();
CType infer_ctype(Context *context, Node *node);

#endif // CTYPE_H_
