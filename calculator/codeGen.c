#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGen.h"

void printAssembly(BTNode *root){
    if(root == NULL || !root->isVar) return;
    printAssembly(root->left);
    printAssembly(root->right);

    int lreg = -1, rreg = -1;
    if(root->left && root->left->isVar) lreg = root->left->reg;
    if(root->right && root->right->isVar) rreg = root->right->reg;

    if (root != NULL) {
        switch (root->data) {
            case ID:
                root->reg = getReg(root->lexeme);
                break;
            case ASSIGN:
                if(root->right->reg < 3) root->reg = getReg(root->lexeme);
                else root->reg = root->right->reg;
                printf("MOV r%d r%d\n", root->left->reg, root->reg);
                break;
            case ADDSUB:
            case MULDIV:
                if (strcmp(root->lexeme, "+") == 0) {
                } else if (strcmp(root->lexeme, "-") == 0) {
                } else if (strcmp(root->lexeme, "*") == 0) {
                } else if (strcmp(root->lexeme, "/") == 0) {
                }
                break;
            case AND:
                break;
            case XOR:
                break;
            case OR:
                break;
            case INCDEC:
                break;
            default:
                ;
        }
    }
    return;
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
            case ADDSUB:
            case MULDIV:
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
                }
                break;
            case AND:
                root->left->val = root->left->val;
                root->right->val = root->right->val;
                root->val = root->left->val & root->right->val;
                break;
            case XOR:
                root->left->val = root->left->val;
                root->right->val = root->right->val;
                root->val = root->left->val ^ root->right->val;
                break;
            case OR:
                root->left->val = root->left->val;
                root->right->val = root->right->val;
                root->val = root->left->val | root->right->val;
                break;
            case INCDEC:
                if(strcmp(root->lexeme, "++") == 0) root->val = Variable(root->right->lexeme)->val = root->right->val + 1;
                else if(strcmp(root->lexeme, "--") == 0) root->val = Variable(root->right->lexeme)->val = root->right->val - 1;
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
        if(Variable(root->lexeme) == NULL) error(NOTFOUND);
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
            case ADDSUB:
            case MULDIV:
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
                }
                break;
            case AND:
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);
                retval = lv & rv;
                break;
            case XOR:
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);
                retval = lv ^ rv;
                break;
            case OR:
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);
                retval = lv | rv;
                break;
            case INCDEC:
                rv = evaluateTree(root->right);
                if(strcmp(root->lexeme, "++") == 0) retval = Variable(root->right->lexeme)->val = rv + 1;
                else if(strcmp(root->lexeme, "--") == 0) retval = Variable(root->right->lexeme)->val = rv - 1;
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

