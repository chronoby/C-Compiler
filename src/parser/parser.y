%{
#include <iostream>
#include "../ast/ast.h"
AstBlock* programBlock;
AstPrimaryExpr* primary;
AstExpr* expre;

extern int yylex();
void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

/* Represents the many different ways we can access our data */
%union {
    int type_int;
    AstPrimaryExpr* primary_expr;
    AstPostfixExpr* postfix_expr;
    AstUnaryExpr* unary_expr;
    AstCastExpr* cast_expr;
    AstMultiplicativeExpr* multi_expr;
    AstAdditiveExpr* add_expr;
    AstShiftExpr* shift_expr;
    AstRelationalExpr* rela_expr;
    AstEqualityExpr* equal_expr;
    AstAndExpr* and_expr;
    AstExclusiveExpr* exclusive_expr;
    AstInclusiveExpr* inclusive_expr;
    AstLogicalAndExpr* log_and_expr;
    AstLogicalOrExpr* log_or_expr;
    AstConditionalExpr* cond_expr;
    AstAssignmentExpr* assign_expr;
    AstExpr* expr;

    AstInt* type_int_node;
    AstBlock* block;
    // AstExpression* expr;
    AstStatement* stmt;
    AstIdentifier* ident;
    std::string* string;
}

%type <primary_expr> primary_expr
%type <postfix_expr> postfix_expr
%type <unary_expr> unary_expr
%type <cast_expr> cast_expr
%type <multi_expr> multiplicative_expr
%type <add_expr> additive_expr
%type <shift_expr> shift_expr
%type <rela_expr> relational_expr
%type <equal_expr> equality_expr
%type <and_expr> and_expr
%type <exclusive_expr> exclusive_or_expr
%type <inclusive_expr> inclusive_or_expr
%type <log_and_expr> logical_and_expr
%type <log_or_expr> logical_or_expr
%type <cond_expr> conditional_expr
%type <assign_expr> assignment_expr

%type <ident> ident
%type <expr> expr 
%type <block> stmts
%type <stmt> stmt var_decl
%type <type_int_node> numeric

%token <string> IDENTIFIER INTEGER HEXI OCTAL FLOAT CHAR STRING
%token TYPE_INT
%token PTR_OP INC_OP DEC_OP
%token SIZEOF

%start translation_unit

%%

primary_expr :
    IDENTIFIER { $$ = new AstPrimaryExpr(*$1); }
    | INTEGER { $$ = new AstPrimaryExpr(AstPrimaryExpr::DataType::INTEGER, *$1); }
    | HEXI { $$ = new AstPrimaryExpr(AstPrimaryExpr::DataType::HEXI, *$1); }
    | OCTAL { $$ = new AstPrimaryExpr(AstPrimaryExpr::DataType::OCTAL, *$1); }
    | FLOAT { $$ = new AstPrimaryExpr(AstPrimaryExpr::DataType::FLOAT, *$1); }
    | CHAR { $$ = new AstPrimaryExpr(AstPrimaryExpr::DataType::CHAR, *$1); }
    | STRING { $$ = new AstPrimaryExpr(AstPrimaryExpr::DataType::STRING, *$1); }
    /*| '('expression')'  { $$ = new AstPrimaryExpr($2); }*/  /* NOTE: different from present expr */
    ;

postfix_expr :
    primary_expr { $$ = new AstPostfixExpr($1); }
    | postfix_expr '[' INTEGER ']' { $$ = new AstPostfixExpr($1, *$3); } /* NOTE: index set as int now */
    /* | postfix_expr '[' expression ']' */ 
    /* | postfix_expr '(' ')' todo
    | postfix_expr '(' argument_expr_list ')'
    | postfix_expr '.' IDENTIFIER
    | postfix_expr PTR_OP IDENTIFIER */
    | postfix_expr INC_OP { $$ = new AstPostfixExpr($1, AstPostfixExpr::OpType::INC); }
    | postfix_expr DEC_OP { $$ = new AstPostfixExpr($1, AstPostfixExpr::OpType::DEC); }
    ;

/* argument_expression_list : 
    assignment_expression
	| argument_expression_list ',' assignment_expression
	; */

unary_expr : 
    postfix_expr { $$ = new AstUnaryExpr($1); }
	| INC_OP unary_expr
	| DEC_OP unary_expr
	/* | unary_operator cast_expr */
	| SIZEOF unary_expr
	/* | SIZEOF '(' type_name ')' */
    ;

cast_expr : 
    unary_expr { $$ = new AstCastExpr($1); }
	/* | '(' type_name ')' cast_expression */
    ;

multiplicative_expr : 
    cast_expr { $$ = new AstMultiplicativeExpr($1); }
	| multiplicative_expr '*' cast_expr
	| multiplicative_expr '/' cast_expr
	| multiplicative_expr '%' cast_expr
    ;

additive_expr : 
multiplicative_expr { $$ = new AstAdditiveExpr($1); }
	| additive_expr '+' multiplicative_expr
	| additive_expr '-' multiplicative_expr
    ;

shift_expr : 
additive_expr { $$ = new AstShiftExpr($1); }
	/* | shift_expr LEFT_OP additive_expr
	| shift_expr RIGHT_OP additive_expr */
    ;

relational_expr : 
    shift_expr { $$ = new AstRelationalExpr($1); }
	| relational_expr '<' shift_expr
	| relational_expr '>' shift_expr
	/* | relational_expr LE_OP shift_expr
	| relational_expr GE_OP shift_expr */
    ;

equality_expr : 
    relational_expr { $$ = new AstEqualityExpr($1); }
	/* | equality_expr EQ_OP relational_expr
	| equality_expr NE_OP relational_expr */
    ;

and_expr : 
    equality_expr { $$ = new AstAndExpr($1); }
	| and_expr '&' equality_expr
	;

exclusive_or_expr : 
    and_expr { $$ = new AstExclusiveExpr($1); }
	| exclusive_or_expr '^' and_expr
	;

inclusive_or_expr : 
    exclusive_or_expr { $$ = new AstInclusiveExpr($1); }
	| inclusive_or_expr '|' exclusive_or_expr
	;

logical_and_expr : 
    inclusive_or_expr { $$ = new AstLogicalAndExpr($1); }
	/* | logical_and_expr AND_OP inclusive_or_expr */
	;

logical_or_expr : 
    logical_and_expr { $$ = new AstLogicalOrExpr($1); }
	/* | logical_or_expr OR_OP logical_and_expr */
	;

conditional_expr : 
    logical_or_expr { $$ = new AstConditionalExpr($1); }
	| logical_or_expr '?' expr ':' conditional_expr
	;

assignment_expr : 
    conditional_expr { $$ = new AstAssignmentExpr($1); }
	/* | unary_expr assignment_operator assignment_expr */
    ;


external_decl :
    decl
    /* | function_def */
    ;

decl :
    decl_specifiers ';'
    | decl_specifiers init_declarator_list ';'
    ;

decl_specifiers :
    : storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| type_specifier
	| type_specifier declaration_specifiers
	| type_qualifier
	| type_qualifier declaration_specifiers
	;

init_declarator_list :
    init_declarator
    | init_declarator_list ',' init_declarator
    ;

init_declarator :
    declarator
	| declarator '=' initializer
    ;

declarator : 
    pointer direct_declarator
	| direct_declarator
    ;

direct_declarator : 
    IDENTIFIER
	| '(' declarator ')'
	| direct_declarator '[' constant_expression ']'
	| direct_declarator '[' ']'
	| direct_declarator '(' parameter_type_list ')'
	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')'
	;

initializer :
    assignment_expression
	| '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	;

/* ------------------------------------------------------------ */    

expr : 
    assignment_expr { $$ = new AstExpr($1); }
	| expr ',' assignment_expr
    ;

translation_unit : 
    /* stmts { programBlock = $1; } */
    expr { expre = $1; }
    ;

stmts : 
    stmt { $$ = new AstBlock(); $$->pushStmt($<stmt>1); }
    | stmts stmt { $1->pushStmt($<stmt>2); }
    ;

stmt : 
    expr ';' { $$ = new AstExprStmt($1); }
    | var_decl
    ;

var_decl :
    TYPE_INT ident ';' { $$ = new AstVariableDeclaration($2); }

ident : 
    IDENTIFIER { $$ = new AstIdentifier(*$1); delete $1; }
    ;

numeric : 
    INTEGER { $$ = new AstInt(atol($1->c_str())); delete $1; }
    ;

%%