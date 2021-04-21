#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGen.h"

void genAssembly(BTNode *root){
    if (root != NULL) {
        switch (root->data) {
            case ID:
                break;
            case INT:
                break;
            case ASSIGN:
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
    return 0;
}

int evaluateTree(BTNode *root) {
    int retval = 0, lv = 0, rv = 0;

    if (root != NULL) {
        switch (root->data) {
            case ID:
                retval = getval(root->lexeme);
                break;
            case INT:
                retval = atoi(root->lexeme);
                break;
            case ASSIGN:
                rv = evaluateTree(root->right);
                retval = setval(root->left->lexeme, rv);
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
                if(strcmp(root->lexeme, "++") == 0) retval = setval(root->right->lexeme, rv + 1);
                else if(strcmp(root->lexeme, "--") == 0) retval = setval(root->right->lexeme, rv - 1);
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

