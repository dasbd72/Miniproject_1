#ifndef __CODEGEN__
#define __CODEGEN__

#include "parser.h"

extern void genAssembly(BTNode *root);
extern void printAssemblyEOF();
extern int printAssembly_v0(BTNode *root);
extern int printAssembly_v1(BTNode *root, int use);

// Count the amount of Variables
extern int preprocess(BTNode *root);

// Evaluate the syntax tree
extern int evaluateTree(BTNode *root);

// Print the syntax tree in prefix
extern void printPrefix(BTNode *root);

#endif // __CODEGEN__
