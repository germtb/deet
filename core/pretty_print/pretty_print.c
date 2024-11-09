#include "../parser/parser.h"
#include "../debug/debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_children(Node *node, int depth, int (*print)(const char *text, ...));
void print_node(Node *node, int depth, int (*print)(const char *text, ...));

void print_children_from_n(Node *node, int depth, int (*print)(const char *text, ...), int n)
{
    NodeList *child = node->head;
    int i = 0;

    while (child != NULL)
    {
        if (i >= n)
        {
            print_node(child->value, depth, print);
        }

        i++;
        child = child->next;
    }
}

void print_n_child(Node *node, int depth, int (*print)(const char *text, ...), int n)
{
    NodeList *child = node->head;

    for (int i = 0; i < n; i++)
    {
        child = child->next;
    }

    if (child != NULL)
    {
        print_node(child->value, depth, print);
    }
}

void print_children(Node *node, int depth, int (*print)(const char *text, ...))
{
    NodeList *child = node->head;

    while (child != NULL)
    {
        print_node(child->value, depth, print);
        child = child->next;
    }
}

void print_op(Node *node, int depth, int (*print)(const char *text, ...), char *op)
{
    print_n_child(node, depth, print, 0);
    print(" %s ", op);
    print_n_child(node, depth, print, 1);
}

void print_node(Node *node, int depth, int (*print)(const char *text, ...))
{
    char indentation[depth + 1];
    fill_array(indentation, '\t', 0, depth);

    NodeList *list = node->head;

    switch (node->type)
    {
    case PostIncrement:
    {
        print_children(node, depth, print);
        print("++");
        break;
    }
    case PostDecrement:
    {
        print_children(node, depth, print);
        print("--");
        break;
    }
    case PreIncrement:
    {
        print("--");
        print_children(node, depth, print);
        break;
    }
    case PreDecrement:
    {
        print("--");
        print_children(node, depth, print);
        break;
    }
    case Comment:
    {
        print(" %s", node->value.s);
        break;
    }
    case Number:
    {
        print("%d", node->value.i);
        break;
    }
    case Decimal:
    {
        print("%f", node->value.f);
        break;
    }
    case Int8:
    {
        print("%di8", node->value.i);
        break;
    }
    case Int16:
    {
        print("%di16", node->value.i);
        break;
    }
    case Int32:
    {
        print("%di32", node->value.i);
        break;
    }
    case Int64:
    {
        print("%di64", node->value.i);
        break;
    }
    case Uint8:
    {
        print("%du8", node->value.i);
        break;
    }
    case Uint16:
    {
        print("%du16", node->value.i);
        break;
    }
    case Uint32:
    {
        print("%du32", node->value.i);
        break;
    }
    case Uint64:
    {
        print("%du64", node->value.i);
        break;
    }
    case Float32:
    {
        print("%fu32", node->value.f);
        break;
    }
    case Float64:
    {
        print("%ff64", node->value.f);
        break;
    }
    case String:
    {
        print("%s", node->value.s);
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
        print("TODO");
        break;
    }
    case StructProperty:
    {
        print("TODO");
        break;
    }
    case Access:
    {
        print_n_child(node, depth, print, 0);
        print("->");
        print_n_child(node, depth, print, 1);
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
    case Zone:
    {
        print_n_child(node, depth, print, 0);
        print(" {");
        print_children_from_n(node, depth + 1, print, 1);
        print("\n%s}", indentation);
        break;
    }
    case Return:
    {
        print("\n%sreturn ", indentation);
        print_children(node, depth, print);
        break;
    }
    case ConstIdentifier:
    {
        print("%s", node->value.s);
        break;
    }
    case VarIdentifier:
    {
        print("%s", node->value.s);
        break;
    }
    case TypeIdentifier:
    {
        print("%s", node->value.s);
        break;
    }
    case EffectIdentifier:
    {
        print("%s", node->value.s);
        break;
    }
    case Function:
    {
        print("fn");
        print("(");
        print_n_child(node, depth, print, 0);
        print(")");

        if (children_count(node) == 3)
        {
            print(": ");
            print_n_child(node, depth, print, 1);
            print(" => ");
            print_n_child(node, depth, print, 2);
        }
        else if (children_count(node) == 2)
        {
            print(" => ");
            print_n_child(node, depth, print, 1);
        }
        else
        {
            print("UNEXPECTED FUNCTION NODE");
        }

        break;
    }
    case FunctionArgs:
    {
        break;
    }
    case FunctionArg:
    {
        break;
    }
    case Call:
    {
        print_n_child(node, depth, print, 0);
        print("(");
        print_children_from_n(node, depth, print, 1);
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
        print_op(node, depth, print, "+");
        break;
    }
    case Sub:
    {
        print_op(node, depth, print, "-");
        break;
    }
    case Mul:
    {
        print_op(node, depth, print, "*");
        break;
    }
    case Div:
    {
        print_op(node, depth, print, "/");
        break;
    }
    case Ge:
    {
        print_op(node, depth, print, ">=");
        break;
    }
    case Gt:
    {
        print_op(node, depth, print, ">");
        break;
    }
    case Se:
    {
        print_op(node, depth, print, "<=");
        break;
    }
    case St:
    {
        print_op(node, depth, print, "<");
        break;
    }
    case Eq:
    {
        print_op(node, depth, print, "==");
        break;
    }
    case Neq:
    {
        print_op(node, depth, print, "!=");
        break;
    }
    case And:
    {
        print_op(node, depth, print, "&&");
        break;
    }
    case Or:
    {
        print_op(node, depth, print, "||");
        break;
    }
    case Xor:
    {
        print_op(node, depth, print, "^");
        break;
    }
    case BitAnd:
    {
        print_op(node, depth, print, "&");
        break;
    }
    case BitOr:
    {
        print_op(node, depth, print, "|");
        break;
    }
    case Not:
    {
        print("!");
        print_children(node, depth, print);
        break;
    }
    case Negative:
    {
        print("-");
        print_children(node, depth, print);
        break;
    }
    case Reassignment:
    {
        print("\n%s", indentation);
        print_n_child(node, depth, print, 0);
        print(" = ");
        print_n_child(node, depth, print, 1);
        break;
    }
    case Declaration:
    {
        if (node->head->value->type == ConstIdentifier)
        {
            print("\n%sconst ", indentation);
        }
        else if (node->head->value->type == VarIdentifier)
        {
            print("\n%svar ", indentation);
        }
        print_n_child(node, depth, print, 0);

        if (children_count(node) == 3)
        {
            print(": ");
            print_n_child(node, depth, print, 1);
            print(" = ");
            print_n_child(node, depth, print, 2);
        }
        else if (children_count(node) == 2)
        {
            print(" = ");
            print_n_child(node, depth, print, 1);
        }
        else
        {
            print("UNEXPECTED DECLARATION NODE");
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
        print_children(node, depth, print);
        print(";");
        break;
    }
    case Program:
    {
        print_children(node, depth, print);
        break;
    }
    case End:
    {
        print("\n");
        break;
    }
    case StructDeclaration:
    {
        break;
    }
    }
}

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

    print_node(node, 0, printf);
    printf("\n");

    fclose(fp);

    return 0;
}

int main()
{
    print_file("./examples/literals.f");
}
