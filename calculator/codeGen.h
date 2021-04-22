#ifndef __CODEGEN__
#define __CODEGEN__

#include "parser.h"

// REG
extern int isRegUsed[REGSIZE];
extern void releaseReg(int i);
extern int getAvailibleReg();

// Generate Assembly Code
extern int printAssembly(BTNode *root);

// Count the amount of Variables
extern void setTable(BTNode *root);

// Prefix the syntax tree
extern void prefixTree(BTNode *root);

// Evaluate the syntax tree
extern int evaluateTree(BTNode *root);

// Print the syntax tree in prefix
extern void printPrefix(BTNode *root);

#endif // __CODEGEN__
