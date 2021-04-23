#ifndef __UTILITIES__
#define __UTILITIES__

#include "parser.h"

#define TBLSIZE 64
#define REGSIZE 8
#define EXTRASIZE 64

// MEM
extern int regToMem[EXTRASIZE];
extern void releaseMem(int i);
extern void clearMem();
extern int getAvailibleMem(int isVar);

// REG
extern int isRegUsed[REGSIZE];
extern void releaseReg(int i);
extern void clearReg();
extern int getAvailibleReg(int isVar);

// Variable
extern Symbol table[TBLSIZE];
extern void makeVariable(char *str);
// Get pointer of variable
extern Symbol *Variable(char *str);
extern Symbol *leastVar();

#endif // __UTILITIES__
