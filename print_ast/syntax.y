%{
#include <stdio.h>
#include "lex.yy.c"
#include "Node.h"
void yyerror(char* msg);
extern Node* ROOT;
int misline[4096]={0};

%}

%union {
	struct NODE* type_node;
}

%token <type_node> IDENTIFIER INTEGER HEXI OCTAL FLOAT CHAR STRING
%token <type_node> VOID TYPE_INT TYPE_CHAR TYPE_FLOAT TYPE_DOUBLE TYPE_LONG TYPE_SHORT TYPE_SIGNED TYPE_UNSIGNED
%token <type_node> PTR_OP INC_OP DEC_OP AND_OP OR_OP EQ_OP NE_OP LE_OP GE_OP ELLIPSIS
%token <type_node> RETURN
%token <type_node> SIZEOF IF ELSE WHILE CONTINUE BREAK
%token <type_node> SEMI COMMA ASSIGN LF RF MULTI LP RP LB RB PLUS MINUS DIV MOD DOT LESS GREATER AND NOT WAVE QUES COLON

%type <type_node> program
%type <type_node> primary_expr
%type <type_node> postfix_expr
%type <type_node> unary_expr
%type <type_node> cast_expr
%type <type_node> multiplicative_expr
%type <type_node> additive_expr
%type <type_node> shift_expr
%type <type_node> relational_expr
%type <type_node> equality_expr
%type <type_node> and_expr
%type <type_node> exclusive_or_expr
%type <type_node> inclusive_or_expr
%type <type_node> logical_and_expr
%type <type_node> logical_or_expr
%type <type_node> conditional_expr
%type <type_node> assignment_expr
%type <type_node> expr
%type <type_node> translation_unit
%type <type_node> external_decl
%type <type_node> decl
%type <type_node> decl_specifiers
%type <type_node> init_declarator_list
%type <type_node> init_declarator
%type <type_node> initializer
%type <type_node> initializer_list
%type <type_node> declarator
%type <type_node> direct_declarator
%type <type_node> type_specifier;
%type <type_node> type_name;

%type <type_node> function_def;
%type <type_node> compound_stmt;

%type <type_node> decl_list;
%type <type_node> stmt_list;
%type <type_node> stmt;
%type <type_node> selection_stmt;
%type <type_node> iter_stmt;
%type <type_node> expr_stmt;
%type <type_node> parameter_type_list;
%type <type_node> parameter_decl;
%type <type_node> parameter_list;
%type <type_node> argument_expr_list;
%type <type_node> jump_stmt;
%type <type_node> unary_op;
%type <type_node> pointer;



%start program
%%
//done
program:  translation_unit {$$=create("Program","",1),add(1,$$,$1);ROOT=$$;}
            ;

//done
translation_unit : 
    external_decl { $$=create("Translation_unit","",1),add(1,$$,$1);}
    | translation_unit external_decl { $$=create("Translation_unit","",1),add(2,$$,$1,$2);}
    ;
    
//done
external_decl :
    decl {  $$=create("External_decl","",1),add(1,$$,$1);}
    | function_def { $$=create("External_decl","",1),add(1,$$,$1);}
    ;
 //done   
decl :
    decl_specifiers SEMI {  $$=create("Decl","",1),add(2,$$,$1,$2);}
    | decl_specifiers init_declarator_list SEMI { $$=create("Decl","",1),add(3,$$,$1,$2,$3);}
    ;
 //done   
decl_specifiers :
	type_specifier { $$=create("Decl_specifiers","",1),add(1,$$,$1);}
	;

//done
type_name:
	type_specifier {  $$=create("Type_name","",1),add(1,$$,$1); }
    	| type_specifier pointer {  $$=create("Type_name","",1),add(2,$$,$1,$2); }
	;
//done	
type_specifier :
    VOID { $$=create("Type_specifier","",1),add(1,$$,$1);}
	| TYPE_CHAR { $$=create("Type_specifier","",1),add(1,$$,$1);}
	| TYPE_SHORT { $$=create("Type_specifier","",1),add(1,$$,$1);}
	| TYPE_INT { $$=create("Type_specifier","",1),add(1,$$,$1);}
	| TYPE_LONG { $$=create("Type_specifier","",1),add(1,$$,$1);}
	| TYPE_FLOAT { $$=create("Type_specifier","",1),add(1,$$,$1);}
	| TYPE_DOUBLE {$$=create("Type_specifier","",1),add(1,$$,$1);}
	| TYPE_SIGNED { $$=create("Type_specifier","",1),add(1,$$,$1);}
	| TYPE_UNSIGNED { $$=create("Type_specifier","",1),add(1,$$,$1);}
	;
//done	
init_declarator_list :
    init_declarator { $$=create("Init_declarator_list","",1),add(1,$$,$1);}
    | init_declarator_list COMMA init_declarator {$$=create("Init_declarator_list","",1),add(3,$$,$1,$2,$3);}
    ;
//done
init_declarator :
    declarator {$$=create("Init_declarator","",1),add(1,$$,$1);}
	| declarator ASSIGN initializer { $$=create("Init_declarator","",1),add(3,$$,$1,$2,$3);}
    ;
//done
declarator : 
    pointer direct_declarator { $$=create("Declarator","",1),add(2,$$,$1,$2);}
	| direct_declarator { $$=create("Declarator","",1),add(1,$$,$1);}
    ;
//done
direct_declarator : 
    IDENTIFIER { $$=create("Direct_declarator","",1),add(1,$$,$1);}
	| direct_declarator LF primary_expr RF {  $$=create("Direct_declarator","",1),add(4,$$,$1,$2, $3,$4);}
	| direct_declarator LP parameter_type_list RP { $$=create("Direct_declarator","",1),add(4,$$,$1,$2, $3,$4); }
	| direct_declarator LP RP { $$=create("Direct_declarator","",1),add(3,$$,$1,$2, $3); }
	;
//done
pointer : 
	MULTI { $$=create("Pointer","",1),add(1,$$,$1);}
	| MULTI pointer {  $$=create("Pointer","",1),add(2,$$,$1,$2);}
	;

//done
primary_expr :
    IDENTIFIER { $$=create("Primary_expr","",1),add(1,$$,$1); }
    | INTEGER { $$=create("Primary_expr","",1),add(1,$$,$1);}
    | HEXI { $$=create("Primary_expr","",1),add(1,$$,$1);}
    | OCTAL { $$=create("Primary_expr","",1),add(1,$$,$1);}
    | FLOAT { $$=create("Primary_expr","",1),add(1,$$,$1);}
    | CHAR { $$=create("Primary_expr","",1),add(1,$$,$1);}
    | STRING { $$=create("Primary_expr","",1),add(1,$$,$1);}
    | LP expr RP  { $$=create("Primary_expr","",1),add(3,$$,$1,$2,$3);} 
    ;
  
 //done 
function_def :
    decl_specifiers declarator decl_list compound_stmt { $$=create("Function_def","",1),add(4,$$,$1,$2,$3,$4);}
	| decl_specifiers declarator compound_stmt { $$=create("Function_def","",1),add(3,$$,$1,$2,$3);}
	| declarator decl_list compound_stmt { $$=create("Function_def","",1),add(3,$$,$1,$2,$3);}
	| declarator compound_stmt { $$=create("Function_def","",1),add(2,$$,$1,$2);}
	;

//done
postfix_expr :
    primary_expr { $$=create("Postfix_expr","",1),add(1,$$,$1);}
    | postfix_expr LF expr RF { $$=create("Postfix_expr","",1),add(4,$$,$1,$2,$3,$4);}
    | postfix_expr LP RP {  $$=create("Postfix_expr","",1),add(3,$$,$1,$2,$3);}
    | postfix_expr LP argument_expr_list RP { $$=create("Postfix_expr","",1),add(4,$$,$1,$2,$3,$4);}
    | postfix_expr INC_OP { $$=create("Postfix_expr","",1),add(2,$$,$1,$2);  }
    | postfix_expr DEC_OP { $$=create("Postfix_expr","",1),add(2,$$,$1,$2);  }
    ;

//done
argument_expr_list : 
    assignment_expr { $$=create("Argument_expr_list","",1),add(1,$$,$1); }
	| argument_expr_list COMMA assignment_expr {$$=create("Argument_expr_list","",1),add(3,$$,$1,$2,$3); }
	; 

//done
unary_expr : 
    postfix_expr {$$=create("Postfix_expr","",1),add(1,$$,$1); }
	| INC_OP unary_expr { $$=create("Postfix_expr","",1),add(2,$$,$1,$2); }
	| DEC_OP unary_expr { $$=create("Postfix_expr","",1),add(2,$$,$1,$2); }
	| unary_op cast_expr {$$=create("Postfix_expr","",1),add(2,$$,$1,$2);}
	| SIZEOF unary_expr{$$=create("Postfix_expr","",1),add(2,$$,$1,$2);}
    ;

//done
unary_op :
    AND { $$=create("Unary_op","",1),add(1,$$,$1);}
	| MULTI { $$=create("Unary_op","",1),add(1,$$,$1);}
	| PLUS { $$=create("Unary_op","",1),add(1,$$,$1);}
	| MINUS { $$=create("Unary_op","",1),add(1,$$,$1);}
	| WAVE { $$=create("Unary_op","",1),add(1,$$,$1);}
	| NOT { $$=create("Unary_op","",1),add(1,$$,$1);}
	;

//done
cast_expr : 
    unary_expr {  $$=create("Cast_expr","",1),add(1,$$,$1); }
   | LP type_name RP cast_expr {$$=create("Cast_expr","",1),add(4,$$,$1,$2,$3,$4);}
    ;

//done
multiplicative_expr : 
    cast_expr {$$=create("Multiplicative_expr","",1),add(1,$$,$1); }
	| multiplicative_expr MULTI cast_expr { $$=create("Multiplicative_expr","",1),add(3,$$,$1,$2,$3);}
	| multiplicative_expr DIV cast_expr { $$=create("Multiplicative_expr","",1),add(3,$$,$1,$2,$3);}
	| multiplicative_expr MOD cast_expr { $$=create("Multiplicative_expr","",1),add(3,$$,$1,$2,$3);}
    ;

//done
additive_expr : 
    multiplicative_expr { $$=create("Additive_expr","",1),add(1,$$,$1);}
	| additive_expr PLUS multiplicative_expr { $$=create("Additive_expr","",1),add(3,$$,$1,$2,$3);}
	| additive_expr MINUS multiplicative_expr { $$=create("Additive_expr","",1),add(3,$$,$1,$2,$3);}
    ;

//done
shift_expr : 
	additive_expr { $$=create("Shift_expr","",1),add(1,$$,$1); }
    ;
//done
relational_expr : 
    shift_expr { $$=create("Relational_expr","",1),add(1,$$,$1);}
	| relational_expr LESS shift_expr { $$=create("Relational_expr","",1),add(3,$$,$1,$2,$3);}
	| relational_expr GREATER shift_expr {$$=create("Relational_expr","",1),add(3,$$,$1,$2,$3);}
	| relational_expr LE_OP shift_expr { $$=create("Relational_expr","",1),add(3,$$,$1,$2,$3);}
	| relational_expr GE_OP shift_expr { $$=create("Relational_expr","",1),add(3,$$,$1,$2,$3);}
    ;
//done
equality_expr : 
    relational_expr {  $$=create("Equality_expr","",1),add(1,$$,$1);}
	| equality_expr EQ_OP relational_expr {$$=create("Equality_expr","",1),add(3,$$,$1,$2,$3);}
	| equality_expr NE_OP relational_expr {$$=create("Equality_expr","",1),add(3,$$,$1,$2,$3);}
    ;
//done
and_expr : 
    equality_expr { $$=create("And_expr","",1),add(1,$$,$1);}
	;
//done
exclusive_or_expr : 
    and_expr { $$=create("Exclusive_or_expr","",1),add(1,$$,$1);}
	;
//done
inclusive_or_expr : 
    exclusive_or_expr { $$=create("Inclusive_or_expr","",1),add(1,$$,$1);}
	;
//done
logical_and_expr : 
    inclusive_or_expr { $$=create("Logical_and_expr","",1),add(1,$$,$1);}
	| logical_and_expr AND_OP inclusive_or_expr {$$=create("Logical_and_expr","",1),add(3,$$,$1,$2,$3);}
	;
//done
logical_or_expr : 
    logical_and_expr { $$=create("Logical_or_expr","",1),add(1,$$,$1);}
	| logical_or_expr OR_OP logical_and_expr { $$=create("Logical_or_expr","",1),add(3,$$,$1,$2,$3);}
	;
//done
conditional_expr : 
    logical_or_expr {$$=create("Conditional_expr","",1),add(1,$$,$1);}
	;
//done
assignment_expr : 
    conditional_expr { $$=create("Assignment_expr","",1),add(1,$$,$1);}
	| unary_expr ASSIGN assignment_expr { $$=create("Assignment_expr","",1),add(3,$$,$1,$2,$3);}
    ;
//done
expr : 
    assignment_expr { $$=create("Expr","",1),add(1,$$,$1);}
    ;
//done
parameter_type_list :
    parameter_list  { $$=create("Parameter_type_list","",1),add(1,$$,$1);}
	| parameter_list COMMA ELLIPSIS { $$=create("Parameter_type_list","",1),add(3,$$,$1,$2,$3);}
	;
//done
parameter_list :
    parameter_decl  { $$=create("Parameter_list","",1),add(1,$$,$1);}
	| parameter_list COMMA parameter_decl { $$=create("Parameter_list","",1),add(3,$$,$1,$2,$3);}
	;
//done
parameter_decl :
    decl_specifiers declarator { $$=create("Parameter_decl","",1),add(2,$$,$1,$2); }
	| decl_specifiers { $$=create("Parameter_decl","",1),add(1,$$,$1);  }
	;
//done
initializer :
    assignment_expr { $$=create("Initializer","",1),add(1,$$,$1);}
	| LB initializer_list RB { $$=create("Initializer","",1),add(3,$$,$1,$2,$3); }
	;
//done
initializer_list :
    initializer {$$=create("Initializer_list","",1),add(1,$$,$1); }
	| initializer_list COMMA initializer { $$=create("Initializer_list","",1),add(3,$$,$1,$2,$3); }
	;
//done
stmt :
	compound_stmt { $$=create("Stmt","",1),add(1,$$,$1);}
    | expr_stmt {$$=create("Stmt","",1),add(1,$$,$1);}
    | selection_stmt { $$=create("Stmt","",1),add(1,$$,$1);}
    | jump_stmt {$$=create("Stmt","",1),add(1,$$,$1);}
    | iter_stmt {$$=create("Stmt","",1),add(1,$$,$1);}
    ;
//done
iter_stmt : 
	WHILE LP expr RP stmt {$$=create("Iter_stmt","",1),add(5,$$,$1,$2,$3,$4,$5);}
	;
//done
selection_stmt :
    IF LP expr RP stmt {  $$=create("Selection_stmt","",1),add(5,$$,$1,$2,$3,$4,$5);}
	| IF LP expr RP stmt ELSE stmt { $$=create("Selection_stmt","",1),add(6,$$,$1,$2,$3,$4,$5,$6);}
    ;
//done
compound_stmt : 
    LB RB {$$=create("Compound_stmt","",1),add(2,$$,$1,$2); }
	| LB stmt_list RB {$$=create("Compound_stmt","",1),add(3,$$,$1,$2,$3);}
	| LB decl_list RB { $$=create("Compound_stmt","",1),add(3,$$,$1,$2,$3);}
	| LB decl_list stmt_list RB { $$=create("Compound_stmt","",1),add(4,$$,$1,$2,$3,$4);}
	;
//done
decl_list :
    decl {$$=create("Decl_list","",1),add(1,$$,$1);}
	| decl_list decl {$$=create("Decl_list","",1),add(2,$$,$1,$2);}
	;
//done
stmt_list : 
    stmt {$$=create("Decl_list","",1),add(1,$$,$1);}
	| stmt_list stmt { $$=create("Decl_list","",1),add(2,$$,$1,$2);}
    ;
//done
expr_stmt :
    SEMI {$$=create("Expr_list","",1),add(1,$$,$1);}
    | expr SEMI { $$=create("Expr_list","",1),add(2,$$,$1,$2);}
    ;
//done
jump_stmt :
    RETURN SEMI { $$=create("Jump_stmt","",1),add(2,$$,$1,$2);}
    | RETURN expr SEMI { $$=create("Jump_stmt","",1),add(3,$$,$1,$2,$3);}
    | CONTINUE SEMI {$$=create("Jump_stmt","",1),add(2,$$,$1,$2); }
    | BREAK SEMI { $$=create("Jump_stmt","",1),add(2,$$,$1,$2); }
    ;

%%

void yyerror(char* msg)
{
        ;
}



