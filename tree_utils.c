#include "parser.h"
#include <stdlib.h>

Node *last_child(Node *node)
{
    NodeList *child = node->head;
    NodeList *prev = NULL;

    while (child != NULL)
    {
        prev = child;
        child = child->next;
    }

    return prev->value;
}

Node *n_child(Node *node, uint32_t count)
{
    NodeList *child = node->head;

    for (int i = 0; i < count; i++)
    {
        child = child->next;
    }

    if (child == NULL)
    {
        return NULL;
    }

    return child->value;
}
