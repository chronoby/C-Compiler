%{
#include <iostream>
#include "../ast/ast.h"
AstBlock *programBlock;

extern int yylex();
void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

/* Represents the many different ways we can access our data */
%union {
    int type_int;
    AstInt* type_int_node;
    AstBlock* block;
    AstExpression* expr;
    AstStatement* stmt;
    AstIdentifier* ident;
    std::string* string;
}

%type <ident> ident
%type <expr> expr 
%type <block> stmts
%type <stmt> stmt var_decl
%type <type_int_node> numeric

%token <string> IDENTIFIER INTEGER
%token INT

%start translation_unit

%%

translation_unit : 
    stmts { programBlock = $1; }
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
    INT ident ';' { $$ = new AstVariableDeclaration($2); }

ident : 
    IDENTIFIER { $$ = new AstIdentifier(*$1); delete $1; }
    ;

numeric : 
    INTEGER { $$ = new AstInt(atol($1->c_str())); delete $1; }
    ;

%%