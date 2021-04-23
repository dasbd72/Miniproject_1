#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"

int sbcount = 0;
Symbol table[TBLSIZE];
int isRegUsed[REGSIZE];
int regToMem[EXTRASIZE];

void releaseReg(int i){
    if(i >= 0 && i <= 7) isRegUsed[i] = 0;
}
void clearReg(){
    int i = 0;
    for(i = 0; i < REGSIZE; i++){
        if(isRegUsed[i] == 2) {
            isRegUsed[i] = 0;
        }
    }
}
int getAvailibleReg(int isVar){
    Symbol *reVar = NULL;
    int i = 0;
    for(i = 0; i < REGSIZE; i++){
        if(!isRegUsed[i]) {
            isRegUsed[i] = isVar ? 1 : 2;
            return i;
        }
    }
    // if no availible, release one
    reVar = leastVar();
    if(reVar != NULL){
        i = reVar->reg;
        printf("MOV [%d] r%d\n", reVar->mem, reVar->reg);
        reVar->reg = -1;
    } else {

    }
    
    return i;
}

void makeVariable(char *str){
    int i = 0;

    for (i = 0; i < sbcount; i++) {
        if (strcmp(str, table[i].name) == 0) {
            return;
        }
    }
    
    if (sbcount >= TBLSIZE)
        error(RUNOUT);
    
    strcpy(table[sbcount].name, str);
    table[sbcount].val = 0;
    table[sbcount].reg = -1;
    table[sbcount].cnt = 1;
    table[sbcount].isVar = 1;
    table[sbcount].mem = sbcount*4;
    sbcount++;
    return;
}

Symbol *Variable(char *str){
    int i = 0;

    for (i = 0; i < sbcount; i++)
        if (strcmp(str, table[i].name) == 0)
            return &table[i];
    return NULL;
}

Symbol *leastVar(){
    int i = 0;
    int minCnt = 0x7fffffff;
    int leastIdx = -1;
    for(i = 0; i < sbcount; i++){
        if(table[i].reg != -1){
            if(table[i].cnt < minCnt){
                minCnt = table[i].cnt;
                leastIdx = i;
            }
        }
    }
    if(leastIdx == -1) return NULL;
    return &table[leastIdx];
}


