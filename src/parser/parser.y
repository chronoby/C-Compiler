%{
#include <iostream>
#include "../ast/ast.h"
AstBlock* programBlock;
AstPrimaryExpr* primary;
AstExpr* expre;
AstExternDecl* exdec;
AstTranslationUnit* unit;

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

    AstTranslationUnit* translation_unit;
    AstExternDecl* extern_decl;
    AstDecl* decl;
    AstDeclSpecifiers* decl_specifiers;
    AstTypeSpecifier* type_specifier;
    AstInitDeclaratorList* init_declarator_list;
    AstInitDeclarator* init_declarator;
    AstInitializer* initializer;
    AstDeclarator* declarator;
    AstDirectDeclarator* direct_declarator;

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

%type<translation_unit> translation_unit
%type<extern_decl> external_decl
%type<decl> decl
%type<decl_specifiers> decl_specifiers
%type<init_declarator_list> init_declarator_list
%type<init_declarator> init_declarator
%type<initializer> initializer
%type<declarator> declarator
%type<direct_declarator> direct_declarator
%type<type_specifier> type_specifier;

%type <ident> ident
%type <expr> expr 
%type <block> stmts
%type <stmt> stmt var_decl
%type <type_int_node> numeric

%token <string> IDENTIFIER INTEGER HEXI OCTAL FLOAT CHAR STRING
%token <string> VOID TYPE_INT TYPE_CHAR TYPE_FLOAT TYPE_DOUBLE TYPE_LONG TYPE_SHORT TYPE_SIGNED TYPE_UNSIGNED
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
	| unary_expr '=' assignment_expr { $$ = new AstAssignmentExpr($1, $3); }
	/* | unary_expr assignment_operator assignment_expr */
    ;

expr : 
    assignment_expr { $$ = new AstExpr($1); }
	| expr ',' assignment_expr
    ;

translation_unit : 
    external_decl { $$ = new AstTranslationUnit($1); unit = $$; }
    | translation_unit external_decl { unit = $1; unit->add_exdec($2); }
    ;

external_decl :
    decl { $$ = new AstExternDecl($1);  }
    /* | function_def */
    ;

decl :
    decl_specifiers ';' { $$ = new AstDecl($1); }
    | decl_specifiers init_declarator_list ';' { $$ = new AstDecl($1, $2); }
    ;

decl_specifiers :
    /* storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| */ type_specifier { $$ = new AstDeclSpecifiers($1); }
	/* | type_specifier declaration_specifiers
	| type_qualifier
	| type_qualifier declaration_specifiers */
	;

type_specifier :
    VOID { $$ = new AstTypeSpecifier(*$1); }
	| TYPE_CHAR { $$ = new AstTypeSpecifier(*$1); }
	| TYPE_SHORT { $$ = new AstTypeSpecifier(*$1); }
	| TYPE_INT { $$ = new AstTypeSpecifier(*$1); }
	| TYPE_LONG { $$ = new AstTypeSpecifier(*$1); }
	| TYPE_FLOAT { $$ = new AstTypeSpecifier(*$1); }
	| TYPE_DOUBLE { $$ = new AstTypeSpecifier(*$1); }
	| TYPE_SIGNED { $$ = new AstTypeSpecifier(*$1); }
	| TYPE_UNSIGNED { $$ = new AstTypeSpecifier(*$1); }
	/* | struct_or_union_specifier
	| enum_specifier
	| TYPE_NAME */
	;

init_declarator_list :
    init_declarator { $$ = new AstInitDeclaratorList($1); }
    | init_declarator_list ',' init_declarator { $$ = $1; $$->add_decl($3); }
    ;

init_declarator :
    declarator { $$ = new AstInitDeclarator($1); }
	| declarator '=' initializer { $$ = new AstInitDeclarator($1, $3); }
    ;

declarator : 
    /* pointer direct_declarator 
	|*/  direct_declarator { $$ = new AstDeclarator($1); }
    ;

direct_declarator : 
    IDENTIFIER { $$ = new AstDirectDeclarator(*$1); }
	/* | '(' declarator ')'
	| direct_declarator '[' constant_expression ']'
	| direct_declarator '[' ']'
	| direct_declarator '(' parameter_type_list ')'
	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')' */
	;

initializer :
    assignment_expr { $$ = new AstInitializer($1); }
	/* | '{' initializer_list '}' */
	/* | '{' initializer_list ',' '}' */
	;

/* ------------------------------------------------------------ */ 

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