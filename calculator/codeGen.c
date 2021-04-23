#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGen.h"
#include "utilities.h"

void genAssembly(BTNode *root){
    preprocess(root);
    printAssembly_v0(root);
    // printAssembly_v1(root, 0);
    clearReg();
}
void printAssemblyEOF(){
    for(int i = 0; i < 3; i++){
        if(!table[i].isVar) printf("MOV r%d %d\n", i, table[i].val);
        else printf("MOV r%d [%d]\n", i, table[i].mem);
    }
    puts("EXIT 0");
}

int printAssembly_v0(BTNode *root){
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
            rr = root->reg = printAssembly_v0(root->right);
            printf("MOV [%d] r%d\n", var->mem, rr);
            break;
        case INCDEC:
            var = Variable(root->right->lexeme);
            rr = root->reg = printAssembly_v0(root->right);
            lr = getAvailibleReg(0);

            printf("MOV r%d %d\n", lr, strcmp(root->lexeme, "++") == 0 ? 1 : -1);
            printf("ADD r%d r%d\n", root->reg, lr);

            releaseReg(lr);
            printf("MOV [%d] r%d\n", var->mem, root->reg);
            break;
        case ADDSUB:
        case MULDIV:
        case AND:
        case XOR:
        case OR:
            lr = root->reg =  printAssembly_v0(root->left);
            rr = printAssembly_v0(root->right);
            if (strcmp(root->lexeme, "+") == 0) printf("ADD ");
            else if (strcmp(root->lexeme, "-") == 0) printf("SUB ");
            else if (strcmp(root->lexeme, "*") == 0) printf("MUL ");
            else if (strcmp(root->lexeme, "/") == 0) printf("DIV ");
            else if (strcmp(root->lexeme, "^") == 0) printf("XOR ");
            else if (strcmp(root->lexeme, "|") == 0) printf("OR ");
            else if (strcmp(root->lexeme, "&") == 0) printf("AND ");

            printf("r%d r%d\n", lr, rr);
            releaseReg(rr);
    }

    return root->reg;
}

// isVar or not, Used or not
// return -1: !use, use&&!isVar
int printAssembly_v1(BTNode *root, int use){
    if(root == NULL) return -1;
    int lr, rr, _num;
    Symbol *var=NULL;

    switch (root->data) {
        case ID:
            if(!use) root->reg = -1;
            else {
                var = Variable(root->lexeme);
                var->cnt--;
                root->isVar = var->isVar;

                if(!root->isVar) {
                    root->reg = -1;
                    root->val = var->val;
                } else {
                    root->reg = getAvailibleReg(0);
                    printf("MOV r%d [%d]\n", root->reg, var->mem);
                }
            }
            break;
        case INT:
            root->isVar = 0;
            root->reg = -1;
            break;
        case ASSIGN:
            var = Variable(root->left->lexeme);
            rr = printAssembly_v1(root->right, 1);
            root->isVar = var->isVar = root->right->isVar;

            if(!root->isVar){
                root->val = var->val = root->right->val;
                root->reg = -1;
            } else {
                printf("MOV [%d] r%d\n", var->mem, rr);
                if(!use) {
                    root->reg = -1;
                    releaseReg(rr);
                } else {
                    root->reg = rr;
                }
            }
            break;
        case INCDEC:
            var = Variable(root->right->lexeme);
            root->isVar = var->isVar;
            
            _num = strcmp(root->lexeme, "++")==0 ? 1 : -1;

            if(!root->isVar) {
                root->val = var->val = var->val + _num;
                root->reg = -1;
            } else {
                rr = getAvailibleReg(0);
                lr = getAvailibleReg(0);
                printf("MOV r%d [%d]\n", rr, var->mem);
                printf("MOV r%d %d\n", lr, _num);
                printf("ADD r%d r%d\n", rr, lr);
                printf("MOV [%d] r%d\n", var->mem, rr);
                releaseReg(lr);
                if(!use) {
                    releaseReg(rr);
                    root->reg = -1;
                } else {
                    root->reg = rr;
                }
            }
            break;
        case ADDSUB:
        case MULDIV:
        case AND:
        case XOR:
        case OR:
            lr = printAssembly_v1(root->left, use);
            rr = printAssembly_v1(root->right, use);
            root->isVar = root->left->isVar || root->right->isVar;
            if(!root->isVar && strcmp(root->lexeme, "/") == 0 && root->right->val == 0){
                root->isVar = 1;
                rr = getAvailibleReg(0);
                printf("MOV r%d %d\n", rr, 0);
            }
            if(!use) root->reg = -1;
            else {
                if(!root->isVar) {
                    root->reg = -1;
                    if (strcmp(root->lexeme, "+") == 0) root->val = root->left->val + root->right->val;
                    else if (strcmp(root->lexeme, "-") == 0) root->val = root->left->val - root->right->val;
                    else if (strcmp(root->lexeme, "*") == 0) root->val = root->left->val * root->right->val;
                    else if (strcmp(root->lexeme, "/") == 0) root->val = root->left->val / root->right->val;
                    else if (strcmp(root->lexeme, "^") == 0) root->val = root->left->val ^ root->right->val;
                    else if (strcmp(root->lexeme, "|") == 0) root->val = root->left->val | root->right->val;
                    else if (strcmp(root->lexeme, "&") == 0) root->val = root->left->val & root->right->val;
                } else {
                    if(lr == -1){
                        root->reg = getAvailibleReg(0);
                        printf("MOV r%d %d\n", root->reg, root->left->val);
                    } 
                    if(rr == -1) {
                        rr = getAvailibleReg(0);
                        printf("MOV r%d %d\n", rr, root->right->val);
                    }

                    if (strcmp(root->lexeme, "+") == 0) printf("ADD ");
                    else if (strcmp(root->lexeme, "-") == 0) printf("SUB ");
                    else if (strcmp(root->lexeme, "*") == 0) printf("MUL ");
                    else if (strcmp(root->lexeme, "/") == 0) printf("DIV ");
                    else if (strcmp(root->lexeme, "^") == 0) printf("XOR ");
                    else if (strcmp(root->lexeme, "|") == 0) printf("OR ");
                    else if (strcmp(root->lexeme, "&") == 0) printf("AND ");

                    printf("r%d r%d\n", lr, rr);
                    releaseReg(rr);
                    root->reg = lr;
                }
            }
    }

    return root->reg;
}

// Build table, cnt appearance, error NOTFOUND, error DIVZERO
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
            rv = preprocess(root->right);
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

