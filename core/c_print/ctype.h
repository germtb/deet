#ifndef CTYPE_H_
#define CTYPE_H_

#include "c_print.h"
#include "../parser/parser.h"

typedef struct CType
{
    char *name;
    char *primitive_name;
    int bytesize;
    bool is_primitive;
} CType;

CType make_primitive_ctype(char *name, int bytesize);
CType make_pointer_ctype(char *name, char *primitive_name);
CType make_ctype_not_found();
CType infer_ctype(Context *context, Node *node);

#endif // CTYPE_H_
