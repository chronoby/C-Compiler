%{
#include <iostream>
#include "../ast/ast.h"
AstBlock *programBlock;
AstPrimaryExpr *primary;

extern int yylex();
void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

/* Represents the many different ways we can access our data */
%union {
    int type_int;
    AstPrimaryExpr* primary_expr;
    AstPostfixExpr* postfix_expr;
    AstUnaryExpr* unary_expr;

    AstInt* type_int_node;
    AstBlock* block;
    AstExpression* expr;
    AstStatement* stmt;
    AstIdentifier* ident;
    std::string* string;
}

%type <primary_expr> primary_expr
%type <postfix_expr> postfix_expr
%type <unary_expr> unary_expr

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
    | postfix_expr '[' INTEGER ']' { $$ = new AstPostfixExpr($1, $3); } /* NOTE: index set as int now */
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

translation_unit : 
    /* stmts { programBlock = $1; } */
    primary_expr { primary = $1; }
    ;

stmts : 
    stmt { $$ = new AstBlock(); $$->pushStmt($<stmt>1); }
    | stmts stmt { $1->pushStmt($<stmt>2); }
    ;

stmt : 
    expr ';' { $$ = new AstExprStmt($1); }
    | var_decl
    ;

expr : 
    ident '=' expr { $$ = new AstAssignment($<ident>1, $3); }
    | numeric '+' numeric { $$ = new AstInt($1->getInt() + $3->getInt()); }
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