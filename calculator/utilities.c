#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"

typedef struct Register{
    int data;
    int marr[MEMSIZE]; // ReleaseReg, 
    int mi;
}Register;

int varCnt = 0;
Symbol varTable[TBLSIZE];
int regReuseIdx = 0;
Register regTable[REGSIZE];
int regToMem[R2MSIZE];
int memTable[MEMSIZE];

/*-------------------------------MEM---------------------------------*/

void releaseMem(int i){
    if(i >= 0 && i < MEMSIZE) memTable[i] = 0;
}
void clearMem(){
    int i = 0;
    for(i = 0; i < MEMSIZE; i++){
        if(memTable[i] == 2) {
            memTable[i] = 0;
        }
    }
}
int getAvailibleMem(int isVar){
    int i = 0;
    for(i = 0; i < MEMSIZE; i++){
        if(memTable[i] == 0) {
            memTable[i] = isVar ? 1 : 2;
            return i;
        }
    }
}

/*-------------------------------REG---------------------------------*/
void initReg(){
    for(int i = 0; i < REGSIZE; i++){
        regTable[i].data = 0;
        regTable[i].mi = 0;
    }
}
void releaseReg(int i){
    if(i >= 0 && i < REGSIZE) {
        if(regTable[i].mi > 0){
            regTable[i].mi--;
            printf("MOV r%d [%d]\n", i, regTable[i].marr[regTable[i].mi]*4);
            releaseMem(regTable[i].marr[regTable[i].mi]);
        } else {
            regTable[i].data = 0;
        }
    }
}
void clearReg(){
    int i = 0;
    for(i = 0; i < REGSIZE; i++){
        if(regTable[i].data == 2) {
            regTable[i].data = 0;
        }
    }
    regReuseIdx = 0;
}
int getAvailibleReg(int isVar){
    Symbol *reVar = NULL;
    int i = 0;
    for(i = 0; i < REGSIZE; i++){
        if(!regTable[i].data) {
            regTable[i].data = isVar ? 1 : 2;
            return i;
        }
    }
    // if no availible, release one
    i = regReuseIdx;
    regTable[regReuseIdx].marr[regTable[regReuseIdx].mi] = getAvailibleMem(0);
    printf("MOV [%d] r%d\n", regTable[regReuseIdx].marr[regTable[regReuseIdx].mi]*4, regReuseIdx);
    regTable[regReuseIdx].mi++;
    ++regReuseIdx;
    if(regReuseIdx >= REGSIZE) regReuseIdx = 0;
    return i;
}

/*-------------------------------VAR---------------------------------*/

void makeVariable(char *str){
    int i = 0;

    for (i = 0; i < varCnt; i++) {
        if (strcmp(str, varTable[i].name) == 0) {
            return;
        }
    }
    
    if (varCnt >= TBLSIZE)
        error(RUNOUT);
    
    strcpy(varTable[varCnt].name, str);
    varTable[varCnt].val = 0;
    varTable[varCnt].reg = -1;
    varTable[varCnt].cnt = 1;
    varTable[varCnt].isVar = 1;
    varTable[varCnt].mem = getAvailibleMem(1);
    varCnt++;
    return;
}

Symbol *Variable(char *str){
    int i = 0;

    for (i = 0; i < varCnt; i++)
        if (strcmp(str, varTable[i].name) == 0)
            return &varTable[i];
    return NULL;
}

// Symbol *leastVar(){
//     int i = 0;
//     int minCnt = 0x7fffffff;
//     int leastIdx = -1;
//     for(i = 0; i < varCnt; i++){
//         if(varTable[i].reg != -1){
//             if(varTable[i].cnt < minCnt){
//                 minCnt = varTable[i].cnt;
//                 leastIdx = i;
//             }
//         }
//     }
//     if(leastIdx == -1) return NULL;
//     return &varTable[leastIdx];
// }


