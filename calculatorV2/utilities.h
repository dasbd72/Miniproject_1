#ifndef __UTILITIES__
#define __UTILITIES__

#include "parser.h"

#define TBLSIZE 64
#define REGSIZE 8
#define R2MSIZE 256
#define MEMSIZE 64

// Structure of the symbol table
typedef struct {
    int val; // printAssembly
    int reg; // printAssembly
    int cnt; // preprocess
    int isVar; // printAssembly
    int mem; // preprocess
    char name[MAXLEN];
} Symbol;

// MEM
extern void releaseMem(int i);
extern void clearMem();
extern int getAvailibleMem(int isVar);

// REG
extern void releaseReg(int i);
extern void clearReg();
extern int getAvailibleReg(int isVar);

// Variable
extern void makeVariable(char *str);
// Get pointer of variable
extern Symbol *Variable(char *str);
extern Symbol *leastVar();

#endif // __UTILITIES__
