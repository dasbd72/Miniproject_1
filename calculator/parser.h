#ifndef __PARSER__
#define __PARSER__

#include "lex.h"
#define TBLSIZE 64
#define REGSIZE 8

// Set PRINTERR to 1 to print error message while calling error()
// Make sure you set PRINTERR to 0 before you submit your code
#define PRINTERR 1
#define PRINTEVAL 1
#define PRINTPRE 1
#define PRINTARROW 1
#define PRINTASSEMBLY 0

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
    int val; // prefixTree
    int reg; // genAssembly
    int cnt; // setTable
    int isVar; // prefixTree
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
// Initialize the symbol table with builtin variables
extern void initTable(void);

// REG
extern int isRegUsed[REGSIZE];
extern void releaseReg(char *str);
extern int getReg(char *str);

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
