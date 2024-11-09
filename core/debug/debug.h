#ifndef DEBUG_H_
#define DEBUG_H_

#include "../zone/zone.h"
#include "../parser/parser.h"

void print_zone(Zone *zone);
void print_node_ast(Node *node, int depth);
void print_ast(char *src);

#endif // DEBUG_H_
