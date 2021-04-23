#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGen.h"

int isRegUsed[REGSIZE];
void releaseReg(int i){
    isRegUsed[i] = 0;
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
    i = reVar->reg;
    printf("MOV [%d] r%d\n", reVar->mem, reVar->reg);
    reVar->reg = -1;
    return i;
}

int printAssembly_e(BTNode *root){
    if(root == NULL) return -1;
    int lr, rr;
    Symbol *var=NULL;

    switch (root->data) {
        case ID:
            root->reg = getAvailibleReg(0);
            var = Variable(root->lexeme);
            printf("MOV r%d [%d]\n", root->reg, var->mem);
            var->cnt--;
            break;
        case INT:
            root->reg = getAvailibleReg(0);
            printf("MOV r%d %d\n", root->reg, root->val);
            break;
        case ASSIGN:
            var = Variable(root->left->lexeme);
            rr = root->reg = printAssembly_e(root->right);
            printf("MOV [%d] r%d\n", var->mem, rr);
            break;
        case INCDEC:
            var = Variable(root->right->lexeme);
            rr = root->reg = printAssembly_e(root->right);

            if (strcmp(root->lexeme, "++") == 0){
                printf("ADD r%d 1\n", rr);
            } else if (strcmp(root->lexeme, "--") == 0) {
                printf("SUB r%d 1\n", rr);
            }
            printf("MOV [%d] r%d\n", var->mem, rr);
            break;
        case ADDSUB:
        case MULDIV:
        case AND:
        case XOR:
        case OR:
            lr = root->reg =  printAssembly_e(root->left);
            rr = printAssembly_e(root->right);
            if (strcmp(root->lexeme, "+") == 0) printf("ADD ");
            else if (strcmp(root->lexeme, "-") == 0) printf("SUB ");
            else if (strcmp(root->lexeme, "*") == 0) printf("MUL ");
            else if (strcmp(root->lexeme, "/") == 0) printf("DIV ");
            else if (strcmp(root->lexeme, "^") == 0) printf("XOR ");
            else if (strcmp(root->lexeme, "|") == 0) printf("OR ");
            else if (strcmp(root->lexeme, "&") == 0) printf("AND ");

            printf("r%d r%d\n", root->reg, rr);
            releaseReg(rr);
    }

    return root->reg;
}

// set reg, print assembly
// return -1 if : !isVar
int printAssembly(BTNode *root){
    if(root == NULL || !root->isVar) return -1;
    
    int lr, rr;
    Symbol *var=NULL;

    switch (root->data) {
        case ID:
            var = Variable(root->lexeme);
            root->reg = var->reg;
            if(var->reg == -1) {
                root->reg = var->reg = getAvailibleReg(1);
                printf("MOV r%d [%d]\n", var->reg, var->mem);
            }
            var->cnt--;
            break;
        case INT:
            break;
        case ASSIGN:
            var = Variable(root->left->lexeme);
            lr = var->reg;
            rr = printAssembly(root->right);
            
            if(rr != -1){
                if(root->right->data != ID && root->right->data != ASSIGN && root->right->data != INCDEC){
                    root->reg = var->reg = rr;
                    releaseReg(lr);
                } else if(lr != -1) {

                } else {
                    lr = getAvailibleReg(1);
                    root->reg = var->reg = lr;
                    printf("MOV r%d r%d\n", lr, rr);
                }
            } else if(rr == -1) {
                if(lr == -1) lr = getAvailibleReg(1);
                root->reg = var->reg = lr;
                printf("MOV r%d %d\n", lr, root->right->val);
            }
            break;
        case INCDEC:
            rr = printAssembly(root->right);

            if (strcmp(root->lexeme, "++") == 0){
                printf("ADD r%d 1\n", rr);
            } else if (strcmp(root->lexeme, "--") == 0) {
                printf("SUB r%d 1\n", rr);
            }
            root->reg = rr;
            break;
        case ADDSUB:
        case MULDIV:
        case AND:
        case XOR:
        case OR:
            lr = root->reg =  printAssembly(root->left);
            rr = printAssembly(root->right);
            if(lr == -1 || root->left->data == ID || root->left->data == ASSIGN || root->left->data == INCDEC){
                root->reg = getAvailibleReg(0);
                if(lr != -1) printf("MOV r%d r%d\n", root->reg, lr);
                else printf("MOV r%d %d\n", root->reg, root->right->val);
            }
            if (strcmp(root->lexeme, "+") == 0) printf("ADD ");
            else if (strcmp(root->lexeme, "-") == 0) printf("SUB ");
            else if (strcmp(root->lexeme, "*") == 0) printf("MUL ");
            else if (strcmp(root->lexeme, "/") == 0) printf("DIV ");
            else if (strcmp(root->lexeme, "^") == 0) printf("XOR ");
            else if (strcmp(root->lexeme, "|") == 0) printf("OR ");
            else if (strcmp(root->lexeme, "&") == 0) printf("AND ");

            if(rr != -1) {
                printf("r%d r%d\n", root->reg, rr);
                if(root->right->data != ID || root->right->data != ASSIGN || root->right->data != INCDEC) releaseReg(rr);
            }
            else printf("r%d %d\n", root->reg, root->right->val);
            break;
    }

    return root->reg;
}

// Build table, cnt appearance, error NOTFOUND, error DIVZERO, set isVar, set val
int preprocess(BTNode *root){
    if(root == NULL) return;

    Symbol *var = NULL;
    int lv, rv;

    switch (root->data) {
        case ID:
            var = Variable(root->lexeme);
            if(var == NULL) 
                error(NOTFOUND);
            var->cnt++;
            root->isVar = 1;
            break;
        case INT:
            root->val = atoi(root->lexeme);
            root->isVar = 0;
            break;
        case ASSIGN:
            rv = preprocess(root->right);
            makeVariable(root->left->lexeme);
            if(rv) return 1;
            root->val = root->right->val;
            root->isVar = 0;
            break;
        case INCDEC:
            root->isVar = 1;
            break;
        case ADDSUB:
        case MULDIV:
        case AND:
        case XOR:
        case OR:
            lv = preprocess(root->left);
            rv = preprocess(root->right);
            if(!rv && root->right->val == 0 && strcmp(root->lexeme, "/")==0) error(DIVZERO);
            if(rv || lv) {
                root->isVar = 1;
                break;
            }
            
            if (strcmp(root->lexeme, "+") == 0) {
                root->val = root->left->val + root->right->val;
            } else if (strcmp(root->lexeme, "-") == 0) {
                root->val = root->left->val - root->right->val;
            } else if (strcmp(root->lexeme, "*") == 0) {
                root->val = root->left->val * root->right->val;
            } else if (strcmp(root->lexeme, "/") == 0) {
                root->val = root->left->val / root->right->val;
            } else if (strcmp(root->lexeme, "&") == 0) {
                root->val = root->left->val & root->right->val;
            } else if (strcmp(root->lexeme, "^") == 0) {
                root->val = root->left->val ^ root->right->val;
            } else if (strcmp(root->lexeme, "|") == 0) {
                root->val = root->left->val | root->right->val;
            }
            root->isVar = 0;
    }
    return root->isVar;
}

int evaluateTree(BTNode *root) {
    int retval = 0, lv = 0, rv = 0;

    if (root != NULL) {
        switch (root->data) {
            case ID:
                retval = Variable(root->lexeme)->val;
                break;
            case INT:
                retval = atoi(root->lexeme);
                break;
            case ASSIGN:
                rv = evaluateTree(root->right);
                retval = Variable(root->left->lexeme)->val = rv;
                break;
            case INCDEC:
                rv = evaluateTree(root->right);
                if(strcmp(root->lexeme, "++") == 0) retval = Variable(root->right->lexeme)->val = rv + 1;
                else if(strcmp(root->lexeme, "--") == 0) retval = Variable(root->right->lexeme)->val = rv - 1;
                break;
            case ADDSUB:
            case MULDIV:
            case AND:
            case XOR:
            case OR:
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);
                if (strcmp(root->lexeme, "+") == 0) {
                    retval = lv + rv;
                } else if (strcmp(root->lexeme, "-") == 0) {
                    retval = lv - rv;
                } else if (strcmp(root->lexeme, "*") == 0) {
                    retval = lv * rv;
                } else if (strcmp(root->lexeme, "/") == 0) {
                    if (rv == 0)
                        error(DIVZERO);
                    retval = lv / rv;
                } else if (strcmp(root->lexeme, "&") == 0) {
                    retval = lv & rv;
                } else if (strcmp(root->lexeme, "^") == 0) {
                    retval = lv ^ rv;
                } else if (strcmp(root->lexeme, "|") == 0) {
                    retval = lv | rv;
                }
                break;
            default:
                retval = 0;
        }
    }
    return retval;
}

void printPrefix(BTNode *root) {
    if (root != NULL) {
        printf("%s ", root->lexeme);
        printPrefix(root->left);
        printPrefix(root->right);
    }
}

