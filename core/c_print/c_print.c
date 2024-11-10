#include "c_print.h"
#include "ctype.h"
#include "../ast/ast.h"
#include "../parser/parser.h"
#include "../hashmap/hashmap.h"
#include "../string/string.h"
#include "../array/array.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print(Context *context, ...)
{
    va_list args;
    va_start(args, context);
    String *string = vstr_template(context->zone, args);
    array_push(context->zone, context->output, string);
    va_end(args);
}

void print_children(Context *context, Node *node, int depth);
void print_node(Context *context, Node *node, int depth);
CType infer_ctype(Context *context, Node *node);

Context make_context(Zone *zone)
{
    Hashmap *types = make_hashmap(zone, 100);
    Hashmap *variables = make_hashmap(zone, 100);

    return (Context){
        .zone = zone,
        .parent = (Parent){InGlobalScope, ""},
        .child = NULL,
        .types = types,
        .variables = variables,
        .depth = 0,
        .output = array(zone, 100)};
}

char *sanetise_identifier(char *identifier)
{
    char c = identifier[0];

    if (c == '@')
    {
        identifier[0] = 'z';
    }

    return identifier;
}

void print_children_from_n(Context *context, Node *node, int depth, int n)
{
    NodeList *child = node->head;
    int i = 0;

    while (child != NULL)
    {
        if (i >= n)
        {
            print_node(context, child->value, depth);
        }

        i++;
        child = child->next;
    }
}

void print_children(Context *context, Node *node, int depth)
{
    NodeList *child = node->head;

    while (child != NULL)
    {
        print_node(context, child->value, depth);
        child = child->next;
    }
}

void print_op(Context *context, Node *node, int depth, char *op)
{
    print_node(context, n_child(node, 0), depth);
    print(context, " %s ", op);
    print_node(context, n_child(node, 1), depth);
}

void print_node(Context *context, Node *node, int depth)
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
    case NStringInterpolation:
        print(context, "TODO");
        print_children(context, node, depth);
        break;
    case NPostIncrement:
    {
        print_children(context, node, depth);
        print(context, "++");
        break;
    }
    case NPostDecrement:
    {
        print_children(context, node, depth);
        print(context, "--");
        break;
    }
    case NPreIncrement:
    {
        print(context, "--");
        print_children(context, node, depth);
        break;
    }
    case NPreDecrement:
    {
        print(context, "--");
        print_children(context, node, depth);
        break;
    }
    case NComment:
    {
        print(context, "\n%s%s", indentation, node->str_value);
        break;
    }
    case NNumber:
    {
        print(context, "%s", node->str_value);
        break;
    }
    case NDecimal:
    {
        print(context, "%s", node->str_value);
        break;
    }
    case NInt8:
    {
        print(context, "%s", node->str_value);
        break;
    }
    case NInt16:
    {
        print(context, "%s", node->str_value);
        break;
    }
    case NInt32:
    {
        print(context, "%s", node->str_value);
        break;
    }
    case NInt64:
    {
        print(context, "%s", node->str_value);
        break;
    }
    case NUint8:
    {
        print(context, "%s", node->str_value);
        break;
    }
    case NUint16:
    {
        print(context, "%s", node->str_value);
        break;
    }
    case NUint32:
    {
        print(context, "%s", node->str_value);
        break;
    }
    case NUint64:
    {
        print(context, "%s", node->str_value);
        break;
    }
    case NFloat32:
    {
        print(context, "%s", node->str_value);
        break;
    }
    case NFloat64:
    {
        print(context, "%s", node->str_value);
        break;
    }
    case NStringLiteral:
    {
        print(context, "str(%s)", node->str_value);
        break;
    }
    case NTrue:
    {
        print(context, "true");
        break;
    }
    case NFalse:
    {
        print(context, "false");
        break;
    }
    case NStruct:
    {
        if (context->parent.type == InStructDeclaration)
        {
            print(context, "{");
            print_children(context, node, depth + 1);
            print(context, "\n%s}", indentation);
        }
        else if (context->parent.type == InVarDeclaration)
        {
            print_children(context, node, depth);
        }
        else
        {
            print(context, "Unexpected case Nin Struct");
        }

        break;
    }
    case NStructProperty:
    {
        if (context->parent.type == InStructDeclaration)
        {
            print(context, "\n%s", indentation);

            CType ctype = infer_ctype(context, n_child(node, 1));

            if (ctype.is_primitive)
            {
                print_node(context, n_child(node, 1), depth);
                print(context, " ");
            }
            else if (strcmp(ctype.name, context->parent.name) == 0)
            {
                print(context, "struct ");
                print_node(context, n_child(node, 1), depth);
                print(context, " *");
            }
            else
            {
                print_node(context, n_child(node, 1), depth);
                print(context, " *");
            }
            print_node(context, n_child(node, 0), depth);
            print(context, ";");
        }
        else if (context->parent.type == InVarDeclaration)
        {
            print(context, "\n%s%s->", indentation, context->parent.name);
            print_node(context, n_child(node, 0), depth);
            print(context, " = ");
            print_node(context, n_child(node, 1), depth);
            print(context, ";");
        }
        else
        {
            print(context, "Unexpected case Nin StructProperty");
        }

        break;
    }
    case NAccess:
    {
        print_node(context, n_child(node, 0), depth);
        print(context, "->");
        print_node(context, n_child(node, 1), depth);
        break;
    }
    case NArray:
    {
        break;
    }
    case NIf:
    {
        break;
    }
    case NElse:
    {
        break;
    }
    case NElseIf:
    {
        break;
    }
    case NMatch:
    {
        break;
    }
    case NMatchCase:
    {
        break;
    }
    case NDefaultCase:
    {
        break;
    }
    case NPattern:
    {
        break;
    }
    case NReturn:
    {
        print(context, "\n%sreturn ", indentation);
        print_children(context, node, depth);
        print(context, ";");
        break;
    }
    case NIdentifier:
    {
        print(context, "%s", sanetise_identifier(node->str_value));
        break;
    }
    case NTypeIdentifier:
    {
        CType *type = hashmap_get(context->types, node->str_value);

        if (type == NULL)
        {
            print(context, "TYPE_NOT_FOUND", node->str_value);
            break;
        }
        else
        {
            print(context, "%s", type->name);
            break;
        }
    }
    case NEffectIdentifier:
    {
        print(context, "%s", sanetise_identifier(node->str_value));
        break;
    }
    case NFunctionDeclaration:
    {
        print(context, "\n");
        CType return_type = infer_ctype(context, n_child(node, 2));

        print(context, "%s ", return_type.name);
        print_node(context, n_child(node, 0), depth);
        print_node(context, n_child(node, 1), depth);
        print_node(context, n_child(node, 3), depth);

        break;
    }
    case NFunction:
    {
        print(context, "fn");
        print(context, "(");
        print_node(context, n_child(node, 0), depth);
        print(context, ")");

        Node *last_node = last_child(node);

        print(context, " {");
        print_node(context, last_node, depth + 1);
        print(context, "\n%s}", indentation);

        break;
    }
    case NFunctionArgs:
    {
        print(context, "(");
        print_children(context, node, depth);
        print(context, ")");
        break;
    }
    case NFunctionArg:
    {
        break;
    }
    case NCall:
    {
        print_node(context, n_child(node, 0), depth);
        print(context, "(");
        print_children_from_n(context, node, depth, 1);
        print(context, ")");
        break;
    }
    case NCallArgs:
    {
        break;
    }
    case NCallArg:
    {
        break;
    }
    case NAdd:
    {
        print_op(context, node, depth, "+");
        break;
    }
    case NSub:
    {
        print_op(context, node, depth, "-");
        break;
    }
    case NMul:
    {
        print_op(context, node, depth, "*");
        break;
    }
    case NDiv:
    {
        print_op(context, node, depth, "/");
        break;
    }
    case NGe:
    {
        print_op(context, node, depth, ">=");
        break;
    }
    case NGt:
    {
        print_op(context, node, depth, ">");
        break;
    }
    case NSe:
    {
        print_op(context, node, depth, "<=");
        break;
    }
    case NSt:
    {
        print_op(context, node, depth, "<");
        break;
    }
    case NEq:
    {
        print_op(context, node, depth, "==");
        break;
    }
    case NNeq:
    {
        print_op(context, node, depth, "!=");
        break;
    }
    case NAnd:
    {
        print_op(context, node, depth, "&&");
        break;
    }
    case NOr:
    {
        print_op(context, node, depth, "||");
        break;
    }
    case NXor:
    {
        print_op(context, node, depth, "^");
        break;
    }
    case NBitAnd:
    {
        print_op(context, node, depth, "&");
        break;
    }
    case NBitOr:
    {
        print_op(context, node, depth, "|");
        break;
    }
    case NNot:
    {
        print(context, "!");
        print_children(context, node, depth);
        break;
    }
    case NNegative:
    {
        print(context, "-");
        print_children(context, node, depth);
        break;
    }
    case NReassignment:
    {
        print(context, "\n%s", indentation);
        print_node(context, n_child(node, 0), depth);
        print(context, " = ");
        print_node(context, n_child(node, 1), depth);
        break;
    }
    case NVarDeclaration:
    case NConstDeclaration:
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

        print(context, "\n%s%s", indentation, ctype.name);

        char *variable_name = sanetise_identifier(n_child(node, 0)->str_value);

        print(context, " %s = ", variable_name);

        if (ctype.is_primitive)
        {

            print_node(context, last_child(node), depth);
            print(context, ";");
        }
        else
        {
            print(context, "malloc(sizeof(%s));\n", ctype.name);
            Parent outer_parent = context->parent;
            context->parent = (Parent){InVarDeclaration, variable_name};
            print_children_from_n(context, node, depth, 2);
            context->parent = outer_parent;
        }

        break;
    }
    case NTypeDeclaration:
    {
        // Store the type in context
        break;
    }
    case NStatement:
    {
        print_children(context, node, depth);
        print(context, ";");
        break;
    }
    case NProgram:
    {
        print_children(context, node, depth);
        break;
    }
    case NEnd:
    {
        break;
    }
    case NStructDeclaration:
    {
        char *name = n_child(node, 0)->str_value;
        CType type = make_pointer_ctype(name);
        // CType *type = malloc(sizeof(CType));
        // type->bytesize = 1;
        // type->name = c_type_name;
        // type->is_primitive = false;
        // type->primitive_name = c_type_primitive_name;

        // Careful I am using a STACK ADDRESS!!
        hashmap_set(context->zone, context->types, name, &type);

        Parent outer_parent = context->parent;
        context->parent = (Parent){InStructDeclaration, name};

        print(context, "\ntypedef struct %s", name);
        print(context, " {");
        print_children_from_n(context, node, depth + 1, 1);
        print(context, "\n} %s;\n", name);

        context->parent = outer_parent;

        break;
    }
    case NBlock:
    {
        print(context, " {");
        print_children(context, node, depth + 1);
        print(context, "\n%s}", indentation);
    }
    case NPipe:
    {
        // TODO
        break;
    }
    case NTypeParameters:
    {
        // TODO
        break;
    }
    case NFor:
    {
        // TODO
        break;
    }
    case NWhile:
    {
        // TODO
        break;
    }
    case NContinue:
    {
        // TODO
        break;
    }
    case NBreak:
    {
        // TODO
        break;
    }
    }
}

char *c_print(char *src, bool add_headers)
{
    Node *node = parse(src);

    // 10Mb of memory
    Zone zone = make_zone(1024 * 10);
    struct Context global_context = make_context(&zone);

    {
        CType u8 = {.name = "uint8_t", .is_primitive = true};
        CType u16 = {.name = "uint16_t", .is_primitive = true};
        CType u32 = {.name = "uint32_t", .is_primitive = true};
        CType u64 = {.name = "uint64_t", .is_primitive = true};

        CType i8 = {.name = "int8_t", .is_primitive = true};
        CType i16 = {.name = "int16_t", .is_primitive = true};
        CType i32 = {.name = "int32_t", .is_primitive = true};
        CType i64 = {.name = "int64_t", .is_primitive = true};

        hashmap_set(&zone, global_context.types, "u8", &u8);
        hashmap_set(&zone, global_context.types, "u16", &u16);
        hashmap_set(&zone, global_context.types, "u32", &u32);
        hashmap_set(&zone, global_context.types, "u64", &u64);

        hashmap_set(&zone, global_context.types, "i8", &i8);
        hashmap_set(&zone, global_context.types, "i16", &i16);
        hashmap_set(&zone, global_context.types, "i32", &i32);
        hashmap_set(&zone, global_context.types, "i64", &i64);

        CType string = {.name = "String", .is_primitive = true};
        hashmap_set(&zone, global_context.types, "String", &string);

        CType void_type = {.name = "void", .is_primitive = true};
        hashmap_set(&zone, global_context.types, "void", &void_type);
    }

    if (add_headers)
    {
        print(&global_context, "#include \"stdlib.h\"\n");
    }

    print_node(&global_context, node, 0);

    String *str_output = array_empty_join(&zone, global_context.output);
    char *output = malloc((str_output->length + 1) * (sizeof(char)));
    memcpy(output, str_output->data, str_output->length);
    output[str_output->length] = '\0';

    free_zone(&zone);
    return output;
}

// char *print_file(const char *filename)
// {
//     FILE *fp;
//     char *src = NULL;

//     int line = 0;
//     int col = 0;

//     fp = fopen(filename, "rw");

//     if (fp == NULL)
//     {
//         printf("Could not open file %s\n", filename);
//         exit(EXIT_FAILURE);
//     }

//     fseek(fp, 0, SEEK_END);
//     long size = ftell(fp);
//     fseek(fp, 0, SEEK_SET);

//     src = malloc(size * sizeof(char));
//     fread(src, 1, size, fp);

//     char *output = c_print(src);

//     printf("%s", output);

//     fclose(fp);

//     return 0;
// }

// int main()
// {
//     print_file("./core/c_print/example_1.dt");
// }
