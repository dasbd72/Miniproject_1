#ifndef __LEX__
#define __LEX__

#define MAXLEN 256

// Token types
typedef enum {
    UNKNOWN, END, ENDFILE,
    INT, ID,
    ADDSUB, MULDIV,
    ASSIGN, INCDEC,
    LPAREN, RPAREN,
    AND, OR, XOR
} TokenSet;

// Test if a token matches the current token 
extern int match(TokenSet token);

// Get the next token
extern void advance(void);

// Get the lexeme of the current token
extern char *getLexeme(void);

#endif // __LEX__
#ifndef __PARSER__
#define __PARSER__


#define TBLSIZE 64
#define REGSIZE 8

// Set PRINTERR to 1 to print error message while calling error()
// Make sure you set PRINTERR to 0 before you submit your code
#define PRINTERR 0
#define PRINTEVAL 0
#define PRINTPRE 0
#define PRINTARROW 0
#define PRINTASSEMBLY 1

// Call this macro to print error message and exit the program
// This will also print where you called it in your program
#define error(errorNum) { \
    if (PRINTERR) \
        fprintf(stderr, "error() called at %s:%d: ", __FILE__, __LINE__); \
    err(errorNum); \
}

// Error types
typedef enum {
    UNDEFINED, MISPAREN, NOTNUMID, NOTFOUND, RUNOUT, NOTLVAL, DIVZERO, SYNTAXERR
} ErrorType;

// Structure of the symbol table
typedef struct {
    int val; // printAssembly
    int reg; // printAssembly
    int cnt; // preprocess
    int isVar; // printAssembly
    int mem;
    char name[MAXLEN];
} Symbol;

// Structure of a tree node
typedef struct _Node {
    TokenSet data;
    int val;
    char lexeme[MAXLEN];
    struct _Node *left; 
    struct _Node *right;

    int reg; // set in printAssembly
    int isVar; // set in prefixTree 
} BTNode;

// The symbol table
extern Symbol table[TBLSIZE];

// Extend table
extern void makeVariable(char *str);
// Get pointer of variable
extern Symbol *Variable(char *str);
extern Symbol *leastVar();
// Initialize the symbol table with builtin variables
extern void initTable(void);

// Make a new node according to token type and lexeme
extern BTNode *makeNode(TokenSet tok, const char *lexe);

// Free the syntax tree
extern void freeTree(BTNode *root);

extern void statement(void);
// new

extern BTNode *assign_expr(void);
extern BTNode *or_expr(void);
extern BTNode *or_expr_tail(BTNode *left);
extern BTNode *xor_expr(void);
extern BTNode *xor_expr_tail(BTNode *left);
extern BTNode *and_expr(void);
extern BTNode *and_expr_tail(BTNode *left);
extern BTNode *addsub_expr(void);
extern BTNode *addsub_expr_tail(BTNode *left);
extern BTNode *muldiv_expr(void);
extern BTNode *muldiv_expr_tail(BTNode *left);
extern BTNode *unary_expr(void);
extern BTNode *factor(void);

// old
// extern BTNode *factor(void);
// extern BTNode *term(void);
// extern BTNode *term_tail(BTNode *left);
// extern BTNode *expr(void);
// extern BTNode *expr_tail(BTNode *left);

// Print error message and exit the program
extern void err(ErrorType errorNum);

#endif // __PARSER__
#ifndef __CODEGEN__
#define __CODEGEN__



// REG
extern int isRegUsed[REGSIZE];
extern int preserveReg[REGSIZE];

extern void releaseReg(int i);
extern void clearReg();
extern int getAvailibleReg(int isVar);

extern int printAssembly_v0(BTNode *root);
extern int printAssembly_v1(BTNode *root, int use);

// Count the amount of Variables
extern int preprocess(BTNode *root);

// Evaluate the syntax tree
extern int evaluateTree(BTNode *root);

// Print the syntax tree in prefix
extern void printPrefix(BTNode *root);

#endif // __CODEGEN__
#include <stdio.h>
#include <string.h>
#include <ctype.h>


static TokenSet getToken(void);
static TokenSet curToken = UNKNOWN;
static char lexeme[MAXLEN];

TokenSet getToken(void)
{
    int i = 0;
    char c = '\0';

    while ((c = fgetc(stdin)) == ' ' || c == '\t');

    if (isdigit(c)) {
        lexeme[0] = c;
        c = fgetc(stdin);
        i = 1;
        while (isdigit(c) && i < MAXLEN) {
            lexeme[i] = c;
            ++i;
            c = fgetc(stdin);
        }
        ungetc(c, stdin);
        lexeme[i] = '\0';
        return INT;
    } else if (c == '+') {
        lexeme[0] = c;
        c = fgetc(stdin);
        if(c == '+'){
            lexeme[1] = c;
            lexeme[2] = '\0';
            return INCDEC;
        } else {
            lexeme[1] = '\0';
            ungetc(c, stdin);
            return ADDSUB;
        }
    } else if (c == '-'){
        lexeme[0] = c;
        c = fgetc(stdin);
        if(c == '-'){
            lexeme[1] = c;
            lexeme[2] = '\0';
            return INCDEC;
        } else {
            lexeme[1] = '\0';
            ungetc(c, stdin);
            return ADDSUB;
        }
    } else if (c == '*' || c == '/') {
        lexeme[0] = c;
        lexeme[1] = '\0';
        return MULDIV;
    } else if (c == '\n') {
        lexeme[0] = '\0';
        return END;
    } else if (c == '=') {
        strcpy(lexeme, "=");
        return ASSIGN;
    } else if (c == '(') {
        strcpy(lexeme, "(");
        return LPAREN;
    } else if (c == ')') {
        strcpy(lexeme, ")");
        return RPAREN;
    } else if (isalpha(c) || c == '_') {
        lexeme[0] = c;
        c = fgetc(stdin);
        int i = 1;
        while((isalpha(c) || isdigit(c) || c == '_') && i < MAXLEN){
            lexeme[i] = c;
            ++i;
            c = fgetc(stdin);
        }
        ungetc(c, stdin);
        lexeme[i] = '\0';
        return ID;
    } else if(c == '&'){
        lexeme[0] = c;
        lexeme[1] = '\0';
        return AND;
    } else if(c == '^'){
        lexeme[0] = c;
        lexeme[1] = '\0';
        return XOR;
    } else if(c == '|'){
        lexeme[0] = c;
        lexeme[1] = '\0';
        return OR;
    } else if (c == EOF) {
        return ENDFILE;
    } else {
        return UNKNOWN;
    }
}

void advance(void) {
    curToken = getToken();
}

int match(TokenSet token) {
    if (curToken == UNKNOWN)
        advance();
    return token == curToken;
}

char *getLexeme(void) {
    return lexeme;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



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
        if(table[i].reg != -1 && !preserveReg[table[i].reg]){
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
    node->isVar = 1;
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
                if(!table[i].isVar) printf("MOV r%d %d\n", i, table[i].val);
                else printf("MOV r%d [%d]\n", i, table[i].mem);
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
            if(PRINTASSEMBLY){
                preprocess(retp);
                printAssembly_v0(retp);
                // printAssembly_v1(retp, 0);
                clearReg();
            }
            if(PRINTEVAL) 
                printf("%d\n", evaluateTree(retp));
            if(PRINTPRE) {
                printf("Prefix traversal: ");
                printPrefix(retp);
                printf("\n");
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int isRegUsed[REGSIZE];
int preserveReg[REGSIZE];
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>



// This package is a calculator
// It works like a Python interpretor
// Example:
// >> y = 2
// >> z = 2
// >> x = 3 * y + 4 / (2 * z)
// It will print the answer of every line
// You should turn it into an expression compiler
// And print the assembly code according to the input

// This is the grammar used in this package
// You can modify it according to the spec and the slide
// statement  :=  ENDFILE | END | expr END
// expr    	  :=  term expr_tail
// expr_tail  :=  ADDSUB term expr_tail | NiL
// term 	  :=  factor term_tail
// term_tail  :=  MULDIV factor term_tail| NiL
// factor	  :=  INT | ADDSUB INT |
//		   	      ID  | ADDSUB ID  | 
//		   	      ID ASSIGN expr |
//		   	      LPAREN expr RPAREN |
//		   	      ADDSUB LPAREN expr RPAREN

int main() {
    initTable();
    if(PRINTARROW) printf(">> ");
    while (1) {
        statement();
    }
    return 0;
}
