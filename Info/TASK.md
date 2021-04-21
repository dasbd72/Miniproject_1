# Task
test in task
## TODO
* (done)variable accepted from char to string

* Binary Operator
    * &, ^, |
* prefix increment/decrement operator
* New variable only valid on left side
* Generate Assembly Language

## Functions
### Current Function
* statement	:= ENDFILE | END | expr END
* expr    	:= term expr_tail
* expr_tail	:= ADDSUB term expr_tail | NiL
* term 		:= factor term_tail
* term_tail	:= MULDIV factor term_tail| NiL
* factor		:= INT | ADDSUB INT | ID  | ADDSUB ID  | ID ASSIGN expr | LPAREN expr RPAREN | ADDSUB LPAREN expr RPAREN

### Target Function
* Priority
    * [*, /] > [+, -] > & > ^ > |

* statement           := END | assign_expr END
* assign_expr         := ID ASSIGN assign_expr | or_expr
* or_expr             := xor_expr or_expr_tail
* or_expr_tail        := OR xor_expr or_expr_tail | NiL
* xor_expr            := and_expr xor_expr_tail
* xor_expr_tail       := XOR and_expr xor_expr_tail | NiL
* and_expr            := addsub_expr and_expr_tail
* and_expr_tail       := AND addsub_expr and_expr_tail | NiL
* addsub_expr         := muldiv_expr addsub_expr_tail
* addsub_expr_tail    := ADDSUB muldiv_expr addsub_expr_tail | NiL
* muldiv_expr         := unary_expr muldiv_expr_tail
* muldiv_expr_tail    := MULDIV unary_expr muldiv_expr_tail | NiL
* unary_expr          := ADDSUB unary_expr | factor
* factor              := INT | ID | INCDEC ID | LPAREN assign_expr RPAREN

