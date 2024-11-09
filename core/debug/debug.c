#include "./debug.h"
#include "../parser/parser.h"
#include "../zone/zone.h"
#include "../string/string.h"

#include <stdio.h>

void print_zone(Zone *zone)
{
    Zone *page = zone;
    int i = 0;

    while (page != NULL)
    {
        // char indentation[i];
        String *indentation = str_len(zone, i);
        str_fill(indentation, '\t');
        printf("%s(Capacity: %zu, pointer: %zu) -> ", indentation->data, page->capacity, page->pointer);
        page = page->next;
        i++;

        if (page == NULL)
        {
            printf("NULL \n");
        }

        printf("\n");
    }
}

void print_node_ast(Node *node, int depth)
{
    char indentation[depth + 1];
    for (int i = 0; i < depth; i++)
    {
        indentation[i] = '\t';
    }
    indentation[depth] = '\0';

    printf("%s", indentation);
    printf("%s", get_node_name(node->type));

    if (node->type == Number || node->type == Int8 || node->type == Int16 || node->type == Int32 || node->type == Int64 || node->type == Uint8 || node->type == Uint16 || node->type == Uint32 || node->type == Uint64 || node->type == Float32 || node->type == Float64)
    {
        printf("(%s)\n", node->str_value);
    }
    else if (node->type == StringLiteral || node->type == TypeIdentifier || node->type == Identifier || node->type == EffectIdentifier)
    {
        printf("(%s)\n", node->str_value);
    }
    else if (node->type == True)
    {
        printf("(true)\n");
    }
    else if (node->type == False)
    {
        printf("(false)\n");
    }
    else
    {
        printf("()\n");
    }

    NodeList *list = node->head;

    while (list != NULL)
    {
        print_node_ast(list->value, depth + 1);
        list = list->next;
    }
}

void print_ast(char *src)
{
    Node *node = parse(src);
    print_node_ast(node, 0);
}
