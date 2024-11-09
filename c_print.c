#include "tree_utils.h"
#include "parser.h"
#include "./core/hashmap/hashmap.h"
#include "./core/string/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct CType
{
    char *name;
    char *primitive_name;
    int bytesize;
    bool is_primitive;
} CType;

CType make_primitive_ctype(char *name, int bytesize)
{
    return (CType){.name = name, .bytesize = bytesize, .is_primitive = true};
}

CType make_pointer_ctype(char *name, char *primitive_name)
{
    return (CType){.name = name, .primitive_name = primitive_name, .bytesize = 1, .is_primitive = false};
}

CType make_ctype_not_found()
{
    return make_pointer_ctype("NOT_FOUND *", "NOT_FOUND");
}

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
} Context;

void print_children(Context *context, Node *node, int depth, int (*print)(const char *text, ...));
void print_node(Context *context, Node *node, int depth, int (*print)(const char *text, ...));
CType infer_ctype(Context *context, Node *node);

Context make_context(Zone *zone)
{
    Hashmap *types = make_hashmap(zone, 100);
    Hashmap *variables = make_hashmap(zone, 100);

    return (Context){
        .zone = zone,
        .parent = NULL,
        .child = NULL,
        .types = types,
        .variables = variables,
        .depth = 0,
        .parent_type = InGlobalScope};
}

char *sanetise_identifier(char *identifier)
{
    char c = identifier[0];

    if (c == '$')
    {
        identifier[0] = 'c';
    }
    else if (c == '@')
    {
        identifier[0] = 'z';
    }
    else if (c == '~')
    {
        identifier[0] = 'v';
    }

    return identifier;
}

void print_children_from_n(Context *context, Node *node, int depth, int (*print)(const char *text, ...), int n)
{
    NodeList *child = node->head;
    int i = 0;

    while (child != NULL)
    {
        if (i >= n)
        {
            print_node(context, child->value, depth, print);
        }

        i++;
        child = child->next;
    }
}

void print_children(Context *context, Node *node, int depth, int (*print)(const char *text, ...))
{
    NodeList *child = node->head;

    while (child != NULL)
    {
        print_node(context, child->value, depth, print);
        child = child->next;
    }
}

void print_op(Context *context, Node *node, int depth, int (*print)(const char *text, ...), char *op)
{
    print_node(context, n_child(node, 0), depth, print);
    print(" %s ", op);
    print_node(context, n_child(node, 1), depth, print);
}

void print_node(Context *context, Node *node, int depth, int (*print)(const char *text, ...))
{
    char indentation[depth + 1];
    for (int i = 0; i < depth; i++)
    {
        indentation[i] = '\t';
    }
    indentation[depth] = '\0';

    NodeList *list = node->head;

    switch (node->type)
    {
    case StringInterpolation:
        print("TODO");
        print_children(context, node, depth, print);
        break;
    case PostIncrement:
    {
        print_children(context, node, depth, print);
        print("++");
        break;
    }
    case PostDecrement:
    {
        print_children(context, node, depth, print);
        print("--");
        break;
    }
    case PreIncrement:
    {
        print("--");
        print_children(context, node, depth, print);
        break;
    }
    case PreDecrement:
    {
        print("--");
        print_children(context, node, depth, print);
        break;
    }
    case Comment:
    {
        print("\n%s%s", indentation, node->str_value);
        break;
    }
    case Number:
    {
        print("%s", node->str_value);
        break;
    }
    case Decimal:
    {
        print("%s", node->str_value);
        break;
    }
    case Int8:
    {
        print("%s", node->str_value);
        break;
    }
    case Int16:
    {
        print("%s", node->str_value);
        break;
    }
    case Int32:
    {
        print("%s", node->str_value);
        break;
    }
    case Int64:
    {
        print("%s", node->str_value);
        break;
    }
    case Uint8:
    {
        print("%s", node->str_value);
        break;
    }
    case Uint16:
    {
        print("%s", node->str_value);
        break;
    }
    case Uint32:
    {
        print("%s", node->str_value);
        break;
    }
    case Uint64:
    {
        print("%s", node->str_value);
        break;
    }
    case Float32:
    {
        print("%s", node->str_value);
        break;
    }
    case Float64:
    {
        print("%s", node->str_value);
        break;
    }
    case StringLiteral:
    {
        print("%s", node->str_value);
        break;
    }
    case True:
    {
        print("true");
        break;
    }
    case False:
    {
        print("false");
        break;
    }
    case Struct:
    {
        if (context->parent_type == InStructDeclaration)
        {
            print("{");
            print_children(context, node, depth + 1, print);
            print("\n%s}", indentation);
        }
        else if (context->parent_type == InVarDeclaration)
        {
            print_children(context, node, depth, print);
        }
        else
        {
            print("Unexpected case in Struct");
        }

        break;
    }
    case StructProperty:
    {
        if (context->parent_type == InStructDeclaration)
        {
            print("\n%s", indentation);
            print_node(context, n_child(node, 1), depth, print);
            print_node(context, n_child(node, 0), depth, print);
            print(";");
        }
        else if (context->parent_type == InVarDeclaration)
        {
            print("\n%s%s->", indentation, context->parent_name);
            print_node(context, n_child(node, 0), depth, print);
            print(" = ");
            print_node(context, n_child(node, 1), depth, print);
            print(";");
        }
        else
        {
            print("Unexpected case in StructProperty");
        }

        break;
    }
    case Access:
    {
        print_node(context, n_child(node, 0), depth, print);
        print("->");
        print_node(context, n_child(node, 1), depth, print);
        break;
    }
    case Tuple:
    {
        break;
    }
    case Array:
    {
        break;
    }
    case If:
    {
        break;
    }
    case Else:
    {
        break;
    }
    case ElseIf:
    {
        break;
    }
    case Match:
    {
        break;
    }
    case MatchCase:
    {
        break;
    }
    case DefaultCase:
    {
        break;
    }
    case Pattern:
    {
        break;
    }
    case ZoneDeclaration:
    {
        print(" {");
        print_children_from_n(context, node, depth + 1, print, 1);
        print("\n%s}", indentation);
        break;
    }
    case Return:
    {
        print("\n%sreturn ", indentation);
        print_children(context, node, depth, print);
        print(";");
        break;
    }
    case ConstIdentifier:
    {
        print("%s", sanetise_identifier(node->str_value));
        break;
    }
    case VarIdentifier:
    {
        print("%s", sanetise_identifier(node->str_value));
        break;
    }
    case TypeIdentifier:
    {
        CType *type = hashmap_get(context->types, node->str_value);

        if (type == NULL)
        {
            print("TYPE_NOT_FOUND", node->str_value);
            break;
        }
        else
        {
            print("%s", type->name);
            break;
        }
    }
    case EffectIdentifier:
    {
        print("%s", sanetise_identifier(node->str_value));
        break;
    }
    case FunctionDeclaration:
    {
        print("\n");
        CType return_type = infer_ctype(context, n_child(node, 2));

        print("%s ", return_type.name);
        print_node(context, n_child(node, 0), depth, print);
        print_node(context, n_child(node, 1), depth, print);
        print_node(context, n_child(node, 3), depth, print);

        break;
    }
    case Function:
    {
        print("fn");
        print("(");
        print_node(context, n_child(node, 0), depth, print);
        print(")");

        Node *last_node = last_child(node);

        if (node->type == ZoneDeclaration)
        {
            print_node(context, last_node, depth, print);
        }
        else
        {
            print(" {\n");
            print_node(context, last_node, depth + 1, print);
            print("\n%s}", indentation);
        }

        break;
    }
    case FunctionArgs:
    {
        print("(");
        print_children(context, node, depth, print);
        print(")");
        break;
    }
    case FunctionArg:
    {
        break;
    }
    case Call:
    {
        print_node(context, n_child(node, 0), depth, print);
        print("(");
        print_children_from_n(context, node, depth, print, 1);
        print(")");
        break;
    }
    case CallArgs:
    {
        break;
    }
    case CallArg:
    {
        break;
    }
    case Add:
    {
        print_op(context, node, depth, print, "+");
        break;
    }
    case Sub:
    {
        print_op(context, node, depth, print, "-");
        break;
    }
    case Mul:
    {
        print_op(context, node, depth, print, "*");
        break;
    }
    case Div:
    {
        print_op(context, node, depth, print, "/");
        break;
    }
    case Ge:
    {
        print_op(context, node, depth, print, ">=");
        break;
    }
    case Gt:
    {
        print_op(context, node, depth, print, ">");
        break;
    }
    case Se:
    {
        print_op(context, node, depth, print, "<=");
        break;
    }
    case St:
    {
        print_op(context, node, depth, print, "<");
        break;
    }
    case Eq:
    {
        print_op(context, node, depth, print, "==");
        break;
    }
    case Neq:
    {
        print_op(context, node, depth, print, "!=");
        break;
    }
    case And:
    {
        print_op(context, node, depth, print, "&&");
        break;
    }
    case Or:
    {
        print_op(context, node, depth, print, "||");
        break;
    }
    case Xor:
    {
        print_op(context, node, depth, print, "^");
        break;
    }
    case BitAnd:
    {
        print_op(context, node, depth, print, "&");
        break;
    }
    case BitOr:
    {
        print_op(context, node, depth, print, "|");
        break;
    }
    case Not:
    {
        print("!");
        print_children(context, node, depth, print);
        break;
    }
    case Negative:
    {
        print("-");
        print_children(context, node, depth, print);
        break;
    }
    case Reassignment:
    {
        print("\n%s", indentation);
        print_node(context, n_child(node, 0), depth, print);
        print(" = ");
        print_node(context, n_child(node, 1), depth, print);
        break;
    }
    case Declaration:
    {
        CType ctype;

        if (children_count(node) == 3)
        {
            ctype = infer_ctype(context, n_child(node, 1));
        }
        else
        {
            ctype = infer_ctype(context, last_child(node));
        }

        if (ctype.name == NULL)
        {
            printf("ERROR: Unknown type\n");
        }

        print("\n%s%s ", indentation, ctype.name);

        char *variable_name = sanetise_identifier(n_child(node, 0)->str_value);
        print(" %s= ", variable_name);

        if (ctype.is_primitive)
        {
            print_node(context, last_child(node), depth, print);
            print(";");
        }
        else
        {
            print("malloc(sizeof(%s));\n", ctype.primitive_name);
            ParentType outer_type = context->parent_type;
            context->parent_type = InVarDeclaration;
            context->parent_name = variable_name;
            print_children_from_n(context, node, depth, print, 2);
            context->parent_type = outer_type;
        }

        break;
    }
    case TypeDeclaration:
    {
        // Store the type in context
        break;
    }
    case Statement:
    {
        print("\n%s", indentation);
        print_children(context, node, depth, print);
        print(";");
        break;
    }
    case Program:
    {
        print_children(context, node, depth, print);
        break;
    }
    case End:
    {
        print("\n");
        break;
    }
    case StructDeclaration:
    {
        char *name = n_child(node, 0)->str_value;
        ParentType outer_type = context->parent_type;
        char *c_type_primitive_name = char_ptr_concat("struct ", name);
        char *c_type_name = char_ptr_concat(c_type_primitive_name, " *");
        CType type = make_pointer_ctype(c_type_name, c_type_primitive_name);
        // CType *type = malloc(sizeof(CType));
        // type->bytesize = 1;
        // type->name = c_type_name;
        // type->is_primitive = false;
        // type->primitive_name = c_type_primitive_name;

        // Careful I am using a STACK ADDRESS!!
        hashmap_set(context->zone, context->types, name, &type);

        context->parent_type = InStructDeclaration;
        print("\nstruct %s", name);
        print(" {");
        print_children_from_n(context, node, depth + 1, print, 1);
        print("\n};\n");
        context->parent_type = outer_type;

        break;
    }
    }
}

char *(*fn)(char *) = sanetise_identifier;

char *print_file(const char *filename)
{
    FILE *fp;
    char *src = NULL;

    int line = 0;
    int col = 0;

    fp = fopen(filename, "rw");

    if (fp == NULL)
    {
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    src = malloc(size * sizeof(char));
    fread(src, 1, size, fp);

    struct Node *node = parse(src);

    // 10Mb of memory
    Zone zone = make_zone(1024 * 10);
    struct Context global_context = make_context(&zone);

    {
        CType u8 = {.name = "uint8_t", .bytesize = 1};
        CType u16 = {.name = "uint16_t", .bytesize = 1};
        CType u32 = {.name = "uint32_t", .bytesize = 1};
        CType u64 = {.name = "uint64_t", .bytesize = 1};

        CType i8 = {.name = "int8_t", .bytesize = 1};
        CType i16 = {.name = "int16_t", .bytesize = 1};
        CType i32 = {.name = "int32_t", .bytesize = 1};
        CType i64 = {.name = "int64_t", .bytesize = 1};

        hashmap_set(&zone, global_context.types, "u8", &u8);
        hashmap_set(&zone, global_context.types, "u16", &u16);
        hashmap_set(&zone, global_context.types, "u32", &u32);
        hashmap_set(&zone, global_context.types, "u64", &u64);

        hashmap_set(&zone, global_context.types, "i8", &i8);
        hashmap_set(&zone, global_context.types, "i16", &i16);
        hashmap_set(&zone, global_context.types, "i32", &i32);
        hashmap_set(&zone, global_context.types, "i64", &i64);

        CType string = {.name = "char *", .bytesize = 1};
        hashmap_set(&zone, global_context.types, "string", &string);
    }

    printf("#include \"stdlib.h\"\n");
    print_node(&global_context, node, 0, printf);
    printf("\n");

    printf("int main() {\n");
    printf("\tcmain();\n");
    printf("}\n");

    fclose(fp);

    return 0;
}

int main()
{
    print_file("./examples/literals.dt");
}

CType infer_ctype(Context *context, Node *node)
{
    switch (node->type)
    {
    case FunctionDeclaration:
        return make_primitive_ctype(NULL, 1);
    case Number:
        return make_primitive_ctype("int32_t", 4);
    case Decimal:
        return make_primitive_ctype("float64_t", 8);
    case Int8:
        return make_primitive_ctype("int8_t", 1);
    case Int16:
        return make_primitive_ctype("int16_t", 2);
    case Int32:
        return make_primitive_ctype("int32_t", 4);
    case Int64:
        return make_primitive_ctype("int64_t", 8);
    case Uint8:
        return make_primitive_ctype("uint8_t", 1);
    case Uint16:
        return make_primitive_ctype("uint16_t", 2);
    case Uint32:
        return make_primitive_ctype("uint32_t", 4);
    case Uint64:
        return make_primitive_ctype("uint64_t", 8);
    case Float32:
        return make_primitive_ctype("float32_t", 4);
    case Float64:
        return make_primitive_ctype("float64_t", 8);
    case StringLiteral:
        return make_primitive_ctype("char*", 8);
    case True:
        return make_primitive_ctype("bool", 1);
    case False:
        return make_primitive_ctype("bool", 1);
    case StructProperty:
        return make_primitive_ctype("TODO: StructProperty", 1);
    case Access:
        return make_primitive_ctype("TODO: Access", 1);
    case Tuple:
        return make_primitive_ctype("TODO: Tuple", 1);
    case Array:
        return make_primitive_ctype("TODO: Array", 1);
    case If:
        return make_primitive_ctype("TODO: If", 1);
    case Else:
        return make_primitive_ctype("TODO: Else", 1);
    case ElseIf:
        return make_primitive_ctype("TODO: ElseIf", 1);
    case Match:
        return make_primitive_ctype("TODO: Match", 1);
    case MatchCase:
        return make_primitive_ctype("TODO: MatchCase", 1);
    case DefaultCase:
        return make_primitive_ctype("TODO: DefaultCase", 1);
    case Pattern:
        return make_primitive_ctype("TODO: Pattern", 1);
    case ZoneDeclaration:
        return make_primitive_ctype("TODO: Zone", 1);
    case Return:
        return make_primitive_ctype("TODO: Return", 1);
    case ConstIdentifier:
        return make_primitive_ctype("TODO: ConstIdentifier", 1);
    case VarIdentifier:
        return make_primitive_ctype("TODO: VarIdentifier", 1);
    case Struct:
    {
        // if (node == NULL)
        // {
        //     printf("YES\n");
        // }
        // else
        // {
        //     printf("NO\n");
        // }

        // print_node_ast(node, 0);

        // printf("Node type: %s", get_node_name(node->type));
        // printf("Node type: %s", get_node_name(node->type));

        // CType *type = hashmap_get(&context->types, node->str_value);

        // if (type == NULL)
        // {
        //     return (CType){.name = "Type not found", .bytesize = 1};
        // }

        // return *type;

        return make_ctype_not_found();
    }
    case TypeIdentifier:
    {
        CType *type = hashmap_get(context->types, node->str_value);

        if (type == NULL)
        {
            return make_ctype_not_found();
        }

        return *type;
    }
    case EffectIdentifier:
        return make_primitive_ctype("TODO: EffectIdentifier", 1);
    case Function:
        return make_primitive_ctype("TODO: Function", 1);
    case FunctionArgs:
        return make_primitive_ctype("TODO: FunctionArgs", 1);
    case FunctionArg:
        return make_primitive_ctype("TODO: FunctionArg", 1);
    case Call:
        return make_primitive_ctype("TODO: Call", 1);
    case CallArgs:
        return make_primitive_ctype("TODO: CallArgs", 1);
    case CallArg:
        return make_primitive_ctype("TODO: CallArg", 1);
    case Add:
        return make_primitive_ctype("TODO: Add", 1);
    case Sub:
        return make_primitive_ctype("TODO: Sub", 1);
    case Mul:
        return make_primitive_ctype("TODO: Mul", 1);
    case Div:
        return make_primitive_ctype("TODO: Div", 1);
    case Ge:
        return make_primitive_ctype("TODO: Ge", 1);
    case Gt:
        return make_primitive_ctype("TODO: Gt", 1);
    case Se:
        return make_primitive_ctype("TODO: Se", 1);
    case St:
        return make_primitive_ctype("TODO: St", 1);
    case Eq:
        return make_primitive_ctype("TODO: Eq", 1);
    case Neq:
        return make_primitive_ctype("TODO: Neq", 1);
    case And:
        return make_primitive_ctype("TODO: And", 1);
    case Or:
        return make_primitive_ctype("TODO: Or", 1);
    case Xor:
        return make_primitive_ctype("TODO: Xor", 1);
    case BitAnd:
        return make_primitive_ctype("TODO: BitAnd", 1);
    case BitOr:
        return make_primitive_ctype("TODO: BitOr", 1);
    case Not:
        return make_primitive_ctype("TODO: Not", 1);
    case Negative:
        return make_primitive_ctype("TODO: Negative", 1);
    case PostIncrement:
        return make_primitive_ctype("TODO: PostIncrement", 1);
    case PostDecrement:
        return make_primitive_ctype("TODO: PostDecrement", 1);
    case PreIncrement:
        return make_primitive_ctype("TODO: PreIncrement", 1);
    case PreDecrement:
        return make_primitive_ctype("TODO: PreDecrement", 1);
    case Reassignment:
        return make_primitive_ctype("TODO: Reassignment", 1);
    case Declaration:
        return make_primitive_ctype("TODO: Declaration", 1);
    case TypeDeclaration:
        return make_primitive_ctype("TODO: TypeDeclaration", 1);
    case Statement:
        return make_primitive_ctype("TODO: Statement", 1);
    case Comment:
        return make_primitive_ctype("TODO: Comment", 1);
    case Program:
        return make_primitive_ctype("TODO: Program", 1);
    case End:
        return make_primitive_ctype("TODO: End", 1);
    case StructDeclaration:
        return make_primitive_ctype("TODO: StructDeclaration", 1);
    case StringInterpolation:
        return make_pointer_ctype("char*", "char*");
    }

    return make_pointer_ctype("TYPE NOT FOUND *", "TYPE NOT FOUND");
}
