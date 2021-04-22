#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGen.h"

int isRegUsed[REGSIZE];
void releaseReg(int i){
    isRegUsed[i] = 0;
}

int getAvailibleReg(){
    Symbol *reVar = NULL;
    int i = 0;
    for(i = 0; i < REGSIZE; i++){
        if(!isRegUsed[i]) {
            isRegUsed[i] = 1;
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
    puts("hey0");
    if(root == NULL || !(root->isVar)) return -1;
    puts("hey");
    int lr, rr;
    Symbol *var=NULL, *reVar=NULL;

    switch (root->data) {
        case ID:
            var = Variable(root->lexeme);
            root->reg = getAvailibleReg();
            printf("MOV r%d [%d]\n", var->reg, var->mem);
            var->cnt--;
            break;
        case INT:
            break;
        case ASSIGN:
            var = Variable(root->left->lexeme);
            rr = root->reg = printAssembly(root->right);
            if(rr != -1) printf("MOV [%d] r%d\n", var->mem, rr);
            else printf("MOV [%d] %d\n", var->mem, root->right->val);
            break;
        case INCDEC:
            rr = printAssembly(root->right);
            root->reg = getAvailibleReg();

            if (strcmp(root->lexeme, "++") == 0){
                printf("ADD r%d 1\n", rr);
            } else if (strcmp(root->lexeme, "--") == 0) {
                printf("SUB r%d 1\n", rr);
            }
            printf("MOV [%d] r%d\n", Variable(root->right->lexeme)->mem, rr);
            root->reg = rr;
            break;
        case ADDSUB:
        case MULDIV:
        case AND:
        case XOR:
        case OR:
            lr = root->reg =  printAssembly(root->left);
            rr = printAssembly(root->right);
            if(lr == -1){
                root->reg = getAvailibleReg();
                printf("MOV r%d %d\n", root->reg, root->left->val);
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
                releaseReg(rr);
            }
            else printf("r%d %d\n", root->reg, root->right->val);
            break;
    }

    return root->reg;
}

// set reg, print assembly
// return -1 if : !isVar
int printAssembly(BTNode *root){
    if(root == NULL || !root->isVar) return -1;
    
    int lr, rr;
    Symbol *var=NULL, *reVar=NULL;

    switch (root->data) {
        case ID:
            var = Variable(root->lexeme);
            root->reg = var->reg;
            if(var->reg == -1) {
                root->reg = var->reg = getAvailibleReg();
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
                    lr = getAvailibleReg();
                    root->reg = var->reg = lr;
                    printf("MOV r%d r%d\n", lr, rr);
                }
            } else if(rr == -1) {
                if(lr == -1) lr = getAvailibleReg();
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
                root->reg = getAvailibleReg();
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

// Set isVar, calc var
int prefixTree(BTNode *root) {
    // Symbol *var = NULL;
    // int lIsv, rIsv;
    // if (root != NULL) {
    //     switch (root->data) {
    //         case ID:
    //             var = Variable(root->lexeme);
    //             if (root->isVar = var->isVar) break;
    //             root->val = var->val;
    //             break;
    //         case INT:
    //             root->val = atoi(root->lexeme);
    //             root->isVar = 0;
    //             break;
    //         case ASSIGN:
    //             rIsv = prefixTree(root->right);
    //             var = Variable(root->left->lexeme);
    //             root->isVar = root->left->isVar = var->isVar = rIsv;
    //             if( !rIsv ) root->val = root->left->val = var->val = root->right->val;
    //             break;
    //         case INCDEC:
    //             rIsv = prefixTree(root->right);
    //             var = Variable(root->right->lexeme);

    //             if(root->isVar = rIsv) break;
    //             if(strcmp(root->lexeme, "++") == 0) root->val = var->val = root->right->val + 1;
    //             else if(strcmp(root->lexeme, "--") == 0) root->val = var->val = root->right->val - 1;
    //             break;
    //         case ADDSUB:
    //         case MULDIV:
    //         case AND:
    //         case XOR:
    //         case OR:
    //             lIsv = prefixTree(root->left);
    //             rIsv = prefixTree(root->right);
    //             if(root->isVar = lIsv || rIsv) break;
                
    //             if (strcmp(root->lexeme, "+") == 0) {
    //                 root->val = root->left->val + root->right->val;
    //             } else if (strcmp(root->lexeme, "-") == 0) {
    //                 root->val = root->left->val - root->right->val;
    //             } else if (strcmp(root->lexeme, "*") == 0) {
    //                 root->val = root->left->val * root->right->val;
    //             } else if (strcmp(root->lexeme, "/") == 0) {
    //                 root->val = root->left->val / root->right->val;
    //             } else if (strcmp(root->lexeme, "&") == 0) {
    //                 root->val = root->left->val & root->right->val;
    //             } else if (strcmp(root->lexeme, "^") == 0) {
    //                 root->val = root->left->val ^ root->right->val;
    //             } else if (strcmp(root->lexeme, "|") == 0) {
    //                 root->val = root->left->val | root->right->val;
    //             }
    //             break;
    //     }
    // }
    // return root->isVar;
}

// Build table, cnt appearance, error NOTFOUND, error DIVZERO, set isVar, set val
int preprocess(BTNode *root){
    if(root == NULL) return;

    Symbol *var = NULL;
    int lv, rv;

    if (root != NULL) {
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
                if(!rv && root->right->val == 0 && strcmp(root->right->lexeme, "/")==0) error(DIVZERO);
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

