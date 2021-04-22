#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "codeGen.h"

int sbcount = 0;
Symbol table[TBLSIZE];

/* -------------------------------Operations------------------------------- */

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

void initTable(void) {
    makeVariable("x");
    makeVariable("y");
    makeVariable("z");
}

BTNode *makeNode(TokenSet tok, const char *lexe) {
    BTNode *node = (BTNode*)malloc(sizeof(BTNode));
    strcpy(node->lexeme, lexe);
    node->data = tok;
    node->val = 0;
    node->left = NULL;
    node->right = NULL;
    node->isVar = 0;
    node->reg = -1;
    return node;
}

void freeTree(BTNode *root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

/* -------------------------------Grammar------------------------------- */

// statement        := END | assign_expr END 
void statement(void) {
    BTNode *retp = NULL;
    int result;

    if (match(ENDFILE)) {
        if(PRINTASSEMBLY){
            for(int i = 0; i < 3; i++){
                if(table[i].reg != -1 && table[i].reg == i){
                    printf("MOV [%d] r%d\n", table[i].mem, table[i].reg);
                    table[i].reg = -1;
                }
            }
            for(int i = 0; i < 3; i++){
                if(table[i].reg != i) 
                    printf("MOV r%d [%d]\n", i, table[i].mem);
            }
            puts("EXIT 0");
        }
        exit(0);
    } else if (match(END)) {
        if(PRINTARROW) printf(">> ");
        advance();
    } else {
        retp = assign_expr();
        if (match(END)) {
            if(PRINTEVAL) 
                printf("%d\n", evaluateTree(retp));
            if(PRINTPRE) {
                printf("Prefix traversal: ");
                printPrefix(retp);
                printf("\n");
            }
            if(PRINTASSEMBLY){
                preprocess(retp);
                prefixTree(retp);
                printAssembly_e(retp);
            }

            freeTree(retp);
            if(PRINTARROW) printf(">> ");
            advance();
        } else {
            error(SYNTAXERR);
        }
    }
}
// assign_expr      := ID ASSIGN assign_expr | or_expr
BTNode* assign_expr(){
    BTNode *retp = or_expr(), *node=NULL;
    if(retp->data == ID && match(ASSIGN)){
        node = makeNode(ASSIGN, getLexeme());
        advance();
        node->left = retp;
        node->right = assign_expr();
        return node;
    } else {
        return retp;
    }
}
// or_expr          := xor_expr or_expr_tail 
BTNode *or_expr(){
    BTNode *left = xor_expr();
    return or_expr_tail(left);
}
// or_expr_tail     := OR xor_expr or_expr_tail | NiL 
BTNode *or_expr_tail(BTNode *left){
    BTNode *node = NULL;

    if(match(OR)){
        node = makeNode(OR, getLexeme());
        advance();
        node->left = left;
        node->right = xor_expr();
        return or_expr_tail(node);
    } else {
        return left;
    }
}
// xor_expr         := and_expr xor_expr_tail 
BTNode *xor_expr(void){
    BTNode *left = and_expr();
    return xor_expr_tail(left);
}
// xor_expr_tail    := XOR and_expr xor_expr_tail | NiL 
BTNode *xor_expr_tail(BTNode *left){
    BTNode *node = NULL;

    if(match(XOR)){
        node = makeNode(XOR, getLexeme());
        advance();
        node->left = left;
        node->right = and_expr();
        return xor_expr_tail(node);
    } else {
        return left;
    }
}
// and_expr         := addsub_expr and_expr_tail
BTNode *and_expr(void){
    BTNode *left = addsub_expr();
    return and_expr_tail(left);
}
// and_expr_tail    := AND addsub_expr and_expr_tail | NiL 
BTNode *and_expr_tail(BTNode *left){
    BTNode *node = NULL;

    if(match(AND)){
        node = makeNode(AND, getLexeme());
        advance();
        node->left = left;
        node->right = addsub_expr();
        return and_expr_tail(node);
    } else {
        return left;
    }
}
// addsub_expr      := muldiv_expr addsub_expr_tail 
BTNode *addsub_expr(void){
    BTNode *left = muldiv_expr();
    return addsub_expr_tail(left);
}
// addsub_expr_tail := ADDSUB muldiv_expr addsub_expr_tail | NiL 
BTNode *addsub_expr_tail(BTNode *left){
    BTNode *node = NULL;

    if(match(ADDSUB)){
        node = makeNode(ADDSUB, getLexeme());
        advance();
        node->left = left;
        node->right = muldiv_expr();
        return addsub_expr_tail(node);
    } else {
        return left;
    }
}
// muldiv_expr      := unary_expr muldiv_expr_tail 
BTNode *muldiv_expr(void){
    BTNode *left = unary_expr();
    return muldiv_expr_tail(left);
}
// muldiv_expr_tail := MULDIV unary_expr muldiv_expr_tail | NiL 
BTNode *muldiv_expr_tail(BTNode *left){
    BTNode *node = NULL;

    if(match(MULDIV)){
        node = makeNode(MULDIV, getLexeme());
        advance();
        node->left = left;
        node->right = unary_expr();
        return muldiv_expr_tail(node);
    } else {
        return left;
    }
}
// unary_expr       := ADDSUB unary_expr | factor 
BTNode *unary_expr(void){
    BTNode *retp = NULL;

    if(match(ADDSUB)){
        retp = makeNode(ADDSUB, getLexeme());
        advance();
        retp->left = makeNode(INT, "0");
        retp->right = unary_expr();
    } else{
        retp = factor();
    }
    return retp;
}
// factor           := INT | ID | INCDEC ID | LPAREN assign_expr RPAREN
BTNode *factor(void){
    BTNode *retp = NULL, *left = NULL;

    if(match(INT)){
        retp = makeNode(INT, getLexeme());
        advance();
    } else if(match(ID)){
        retp = makeNode(ID, getLexeme());
        advance();
    } else if(match(INCDEC)){
        retp = makeNode(INCDEC, getLexeme());
        advance();

        if(match(ID)){
            retp->right = makeNode(ID, getLexeme());
            advance();
        } else {
            error(NOTNUMID);
        }
    } else if(match(LPAREN)){
        advance();
        retp = assign_expr();

        if (match(RPAREN)) {
            advance();
        }
        else {
            error(MISPAREN);
        }
    } else {
        error(NOTNUMID);
    }
    return retp;
}

/* -------------------------------Error------------------------------- */
void err(ErrorType errorNum) {
    if (PRINTERR) {
        fprintf(stderr, "error: ");
        switch (errorNum) {
            case MISPAREN:
                fprintf(stderr, "mismatched parenthesis\n");
                break;
            case NOTNUMID:
                fprintf(stderr, "number or identifier expected\n");
                break;
            case NOTFOUND:
                fprintf(stderr, "variable not defined\n");
                break;
            case RUNOUT:
                fprintf(stderr, "out of memory\n");
                break;
            case NOTLVAL:
                fprintf(stderr, "lvalue required as an operand\n");
                break;
            case DIVZERO:
                fprintf(stderr, "divide by constant zero\n");
                break;
            case SYNTAXERR:
                fprintf(stderr, "syntax error\n");
                break;
            default:
                fprintf(stderr, "undefined error\n");
                break;
        }
    }
    else puts("EXIT 1");
    exit(0);
}
