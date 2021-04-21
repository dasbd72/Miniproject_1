#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGen.h"

int printAssembly(BTNode *root){
    if(root == NULL || !root->isVar) return -1;

    int lr, rr;
    lr = printAssembly(root->left);
    rr = printAssembly(root->right);
    
    switch (root->data) {
        case ID:
            
            break;
        case ASSIGN:
            
            break;
        case INCDEC:

            break;
        case ADDSUB:
        case MULDIV:
        case AND:
        case XOR:
        case OR:
            if (strcmp(root->lexeme, "+") == 0) {
            } else if (strcmp(root->lexeme, "-") == 0) {
            } else if (strcmp(root->lexeme, "*") == 0) {
            } else if (strcmp(root->lexeme, "/") == 0) {
            } else if (strcmp(root->lexeme, "&") == 0) {
            } else if (strcmp(root->lexeme, "^") == 0) {
            } else if (strcmp(root->lexeme, "|") == 0) {
            }
            break;
    }

    return -1;
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

