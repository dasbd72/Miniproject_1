#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGen.h"

int isRegUsed[REGSIZE];
void releaseReg(int i){
    isRegUsed[i] = 0;
}
int getAvailibleReg(){
    int i = 0;
    for(i = 0; i < REGSIZE; i++){
        if(!isRegUsed[i]) return i;
    }
    return -1;
}

int printAssembly(BTNode *root){
    if(root == NULL || !root->isVar) return -1;

    int lr, rr;
    Symbol *var=NULL, *reVar=NULL;
    lr = printAssembly(root->left);
    rr = printAssembly(root->right);
    
    switch (root->data) {
        case ID:
            var = Variable(root->lexeme);
            if(var->reg != -1) {
                root->reg = var->reg;
            }
            else {
                root->reg = getAvailibleReg();
                if(root->reg != -1) var->reg = root->reg;
                else {
                    reVar = leastVar();
                    printf("MOV [%d] r%d\n", reVar->mem, reVar->reg);
                    root->reg = reVar->reg;
                    reVar->reg = -1;
                }
                printf("MOV r%d [%d]\n", root->reg, var->mem);
            }
            break;
        case ASSIGN:
            printf("MOV r%d r%d\n", lr, rr);
            releaseReg(rr);
            root->reg = lr;
            break;
        case INCDEC:
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
            root->reg = lr;
            if (strcmp(root->lexeme, "+") == 0) {
                printf("ADD r%d r%d\n", lr, rr);
            } else if (strcmp(root->lexeme, "-") == 0) {
                printf("SUB r%d r%d\n", lr, rr);
            } else if (strcmp(root->lexeme, "*") == 0) {
                printf("MUL r%d r%d\n", lr, rr);
            } else if (strcmp(root->lexeme, "/") == 0) {
                printf("DIV r%d r%d\n", lr, rr);
            } else if (strcmp(root->lexeme, "^") == 0) {
                printf("XOR r%d r%d\n", lr, rr);
            } else if (strcmp(root->lexeme, "|") == 0) {
                printf("OR r%d r%d\n", lr, rr);
            } else if (strcmp(root->lexeme, "&") == 0) {
                printf("AND r%d r%d\n", lr, rr);
            }
            break;
    }

    return root->reg;
}

void prefixTree(BTNode *root) {

    if (root != NULL && !root->isVar) {
        prefixTree(root->left);
        prefixTree(root->right);
        if( (root->right && root->right->isVar) || (root->left && root->left->isVar) ) {
            root->isVar = 1;
            if(root->data == ASSIGN){
                Variable(root->left->lexeme)->isVar = 1;
                root->left->isVar = 1;
            }
            return;
        }

        switch (root->data) {
            case ID:
                if(Variable(root->lexeme)->isVar){
                    root->isVar = 1;
                } else {
                    root->val = Variable(root->lexeme)->val;
                }
                break;
            case INT:
                root->val = atoi(root->lexeme);
                break;
            case ASSIGN:
                root->left->isVar = 0;
                Variable(root->left->lexeme)->isVar = 0;
                root->val = Variable(root->left->lexeme)->val = root->right->val;
                break;
            case INCDEC:
                if(strcmp(root->lexeme, "++") == 0) root->val = Variable(root->right->lexeme)->val = root->right->val + 1;
                else if(strcmp(root->lexeme, "--") == 0) root->val = Variable(root->right->lexeme)->val = root->right->val - 1;
                break;
            case ADDSUB:
            case MULDIV:
            case AND:
            case XOR:
            case OR:
                if (strcmp(root->lexeme, "+") == 0) {
                    root->val = root->left->val + root->right->val;
                } else if (strcmp(root->lexeme, "-") == 0) {
                    root->val = root->left->val - root->right->val;
                } else if (strcmp(root->lexeme, "*") == 0) {
                    root->val = root->left->val * root->right->val;
                } else if (strcmp(root->lexeme, "/") == 0) {
                    if (root->right->val == 0)
                        error(DIVZERO);
                    root->val = root->left->val / root->right->val;
                } else if (strcmp(root->lexeme, "&") == 0) {
                    root->val = root->left->val & root->right->val;
                } else if (strcmp(root->lexeme, "^") == 0) {
                    root->val = root->left->val ^ root->right->val;
                } else if (strcmp(root->lexeme, "|") == 0) {
                    root->val = root->left->val | root->right->val;
                }
                break;
        }
    }
    return;
}

void setTable(BTNode *root){
    if(root == NULL) return;
    if(root->data == ASSIGN){
        setTable(root->right);
        makeVariable(root->left->lexeme);
    } else if(root->data == ID){
        if(Variable(root->lexeme) == NULL) 
            error(NOTFOUND);
        Variable(root->lexeme)->cnt++;
    } else {
        setTable(root->left);
        setTable(root->right);
    }
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

