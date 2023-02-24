#ifndef LLP2_PRINTER_H
#define LLP2_PRINTER_H

#include "tree.h"
#include <stdlib.h>

void print_tree(node *root);
//char *to_string(node *n, int tabs);
void to_string(char* buf, node *n, int tab);
#endif //LLP2_PRINTER_H
