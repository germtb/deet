#ifndef TREE_UTILS_H_
#define TREE_UTILS_H_

#include "parser.h"
#include <stdlib.h>

Node *last_child(Node *node);
Node *n_child(Node *node, uint32_t count);
void for_each_child(Node *node, void (*callback)(Node *));

#endif /* TREE_UTILS_H_ */
