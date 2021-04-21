#ifndef __CODEGEN__
#define __CODEGEN__

#include "parser.h"

// Generate Assembly Code
extern void printAssembly(BTNode *root);

// Count the amount of Variables
extern void countTree(BTNode *root);

// Prefix the syntax tree
extern void prefixTree(BTNode *root);

// Evaluate the syntax tree
extern int evaluateTree(BTNode *root);

// Print the syntax tree in prefix
extern void printPrefix(BTNode *root);

#endif // __CODEGEN__
