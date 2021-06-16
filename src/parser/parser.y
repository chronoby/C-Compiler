%{
#include <iostream>
#include "../ast/ast.h"
AstTranslationUnit* unit;

extern int yylex();
void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

/* Represents the many different ways we can access our data */
%union {
    int type_int;
    char* str;

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

    AstFunctionDef* function_def;
    AstCompoundStmt* compound_stmt;
    AstStmt* stmt;
    AstSelectionStmt* selection_stmt;
    AstIterStmt* iter_stmt;
    AstDeclList* decl_list;
    AstStmtList* stmt_list;
    AstExprStmt* expr_stmt;
    AstParameterTypeList* parameter_type_list;
    AstParameterDecl* parameter_decl;
    AstParameterList* parameter_list;
    AstArgumentExprList* argument_expr_list;
    AstJumpStmt* jump_stmt;

    AstUnaryOp* unary_op;
    AstPointer* pointer;

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
%type <expr> expr
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

%type<function_def> function_def;
%type<compound_stmt> compound_stmt;

%type<decl_list> decl_list;
%type<stmt_list> stmt_list;
%type<stmt> stmt;
%type<selection_stmt> selection_stmt
%type<iter_stmt> iter_stmt
%type<expr_stmt> expr_stmt;
%type<parameter_type_list> parameter_type_list;
%type<parameter_decl> parameter_decl;
%type<parameter_list> parameter_list;
%type<argument_expr_list> argument_expr_list;
%type<jump_stmt> jump_stmt;
%type<unary_op> unary_op;
%type<pointer> pointer;

%token <string> IDENTIFIER INTEGER HEXI OCTAL FLOAT CHAR STRING
%token <string> VOID TYPE_INT TYPE_CHAR TYPE_FLOAT TYPE_DOUBLE TYPE_LONG TYPE_SHORT TYPE_SIGNED TYPE_UNSIGNED
%token PTR_OP INC_OP DEC_OP AND_OP OR_OP EQ_OP NE_OP LE_OP GE_OP ELLIPSIS
%token RETURN
%token SIZEOF IF ELSE WHILE CONTINUE BREAK

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
    | '('expr')'  { $$ = new AstPrimaryExpr($2); } 
    ;

postfix_expr :
    primary_expr { $$ = new AstPostfixExpr($1); }
    | postfix_expr '[' expr ']' { $$ = new AstPostfixExpr($1, $3); }

    /* | postfix_expr '[' expression ']' */ 
    | postfix_expr '(' ')' { 
        $$ = $1; 
        $$->setExprType(AstPostfixExpr::ExprType::FUNC);
    }
    | postfix_expr '(' argument_expr_list ')' {
        $$ = $1;
        $$->setExprType(AstPostfixExpr::ExprType::FUNC_PARAM);
        $$->argument_expr_list = $3;
    }
    
    /*| postfix_expr '.' IDENTIFIER
    | postfix_expr PTR_OP IDENTIFIER */
    | postfix_expr INC_OP { 
        $$ = $1;
        $$->setExprType(AstPostfixExpr::ExprType::OP);
        $$->setOpType(AstPostfixExpr::OpType::INC);
    }
    | postfix_expr DEC_OP { 
        $$ = $1;
        $$->setExprType(AstPostfixExpr::ExprType::OP);
        $$->setOpType(AstPostfixExpr::OpType::DEC);
    }
    ;

argument_expr_list : 
    assignment_expr { $$ = new AstArgumentExprList($1); }
	| argument_expr_list ',' assignment_expr { $$ = $1; $$->add_expr($3); }
	; 

unary_expr : 
    postfix_expr { $$ = new AstUnaryExpr($1); }
	| INC_OP unary_expr
	| DEC_OP unary_expr
	| unary_op cast_expr { $$ = new AstUnaryExpr($2, $1); }
	| SIZEOF unary_expr
	/* | SIZEOF '(' type_name ')' */
    ;

unary_op :
    '&' { $$ = new AstUnaryOp(AstUnaryOp::OpType::AND); }
	| '*' { $$ = new AstUnaryOp(AstUnaryOp::OpType::STAR); }
	| '+' { $$ = new AstUnaryOp(AstUnaryOp::OpType::PLUS); }
	| '-' { $$ = new AstUnaryOp(AstUnaryOp::OpType::MINUS); }
	| '~' { $$ = new AstUnaryOp(AstUnaryOp::OpType::STAR); }
	| '!' { $$ = new AstUnaryOp(AstUnaryOp::OpType::NOT); }
	;

cast_expr : 
    unary_expr { $$ = new AstCastExpr($1); }
	/* | '(' type_name ')' cast_expression */
    ;

multiplicative_expr : 
    cast_expr { $$ = new AstMultiplicativeExpr($1); }
	| multiplicative_expr '*' cast_expr { $$ = new AstMultiplicativeExpr($1, AstMultiplicativeExpr::OpType::MUL, $3); }
	| multiplicative_expr '/' cast_expr { $$ = new AstMultiplicativeExpr($1, AstMultiplicativeExpr::OpType::DIV, $3); }
	| multiplicative_expr '%' cast_expr { $$ = new AstMultiplicativeExpr($1, AstMultiplicativeExpr::OpType::MOD, $3); }
    ;

additive_expr : 
    multiplicative_expr { $$ = new AstAdditiveExpr($1); }
	| additive_expr '+' multiplicative_expr { $$ = new AstAdditiveExpr($1, AstAdditiveExpr::OpType::PLUS, $3); }
	| additive_expr '-' multiplicative_expr { $$ = new AstAdditiveExpr($1, AstAdditiveExpr::OpType::MINUS, $3); }
    ;

shift_expr : 
additive_expr { $$ = new AstShiftExpr($1); }
	/* | shift_expr LEFT_OP additive_expr
	| shift_expr RIGHT_OP additive_expr */
    ;

relational_expr : 
    shift_expr { $$ = new AstRelationalExpr($1); }
	| relational_expr '<' shift_expr { $$ = new AstRelationalExpr($1, AstRelationalExpr::OpType::LESS, $3); }
	| relational_expr '>' shift_expr { $$ = new AstRelationalExpr($1, AstRelationalExpr::OpType::GREATER, $3); }
	| relational_expr LE_OP shift_expr { $$ = new AstRelationalExpr($1, AstRelationalExpr::OpType::LE, $3); }
	| relational_expr GE_OP shift_expr { $$ = new AstRelationalExpr($1, AstRelationalExpr::OpType::GE, $3); }
    ;

equality_expr : 
    relational_expr { $$ = new AstEqualityExpr($1); }
	| equality_expr EQ_OP relational_expr { $$ = new AstEqualityExpr($1, AstEqualityExpr::OpType::EQ, $3); }
	| equality_expr NE_OP relational_expr { $$ = new AstEqualityExpr($1, AstEqualityExpr::OpType::NE, $3); }
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
	| logical_and_expr AND_OP inclusive_or_expr { $$ = new AstLogicalAndExpr($1, $3); }
	;

logical_or_expr : 
    logical_and_expr { $$ = new AstLogicalOrExpr($1); }
	| logical_or_expr OR_OP logical_and_expr { $$ = new AstLogicalOrExpr($1, $3); }
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

decl :
    decl_specifiers ';' { $$ = new AstDecl($1); }
    | decl_specifiers init_declarator_list ';' { $$ = new AstDecl($1, $2); }
    ;

decl_specifiers :
    /* storage_class_specifier
	| storage_class_specifier decl_specifiers
	| */ type_specifier { $$ = new AstDeclSpecifiers($1); }
	/* | type_specifier decl_specifiers
	| type_qualifier
	| type_qualifier decl_specifiers */
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
    pointer direct_declarator { $$ = new AstDeclarator($1, $2); }
	|  direct_declarator { $$ = new AstDeclarator($1); }
    ;

direct_declarator : 
    IDENTIFIER { $$ = new AstDirectDeclarator(*$1, AstDirectDeclarator::DeclaratorType::ID); }
	/* | '(' declarator ')' */
	| direct_declarator '[' primary_expr ']' { $$ = $1; $$->prime_expr = $3; $$->setType(AstDirectDeclarator::DeclaratorType::BR); }
	/* | direct_declarator '[' ']'
	| direct_declarator '(' identifier_list ')'  */
	| direct_declarator '(' parameter_type_list ')' { $$ = $1; $$->param_type_list=$3; $$->setType(AstDirectDeclarator::DeclaratorType::FUNC_PARAM);  }
	| direct_declarator '(' ')' { $$ = $1; $$->setType(AstDirectDeclarator::DeclaratorType::FUNC_EMPTY); }
	;

pointer
	: '*' { $$ = new AstPointer();}
	/* | '*' type_qualifier_list */
	| '*' pointer { $$ = new AstPointer($2); }
	/* | '*' type_qualifier_list pointer */
	;

parameter_type_list :
    parameter_list  { $$ = new AstParameterTypeList($1, false); }
	| parameter_list ',' ELLIPSIS { $$ = new AstParameterTypeList($1, true); }
	;

parameter_list :
    parameter_decl  { $$ = new AstParameterList($1); }
	| parameter_list ',' parameter_decl { $$ = $1; $$->add_param_decl($3); }
	;

parameter_decl :
    decl_specifiers declarator { $$ = new AstParameterDecl($1, $2); }
	/* | decl_specifiers abstract_declarator */
	| decl_specifiers { $$ = new AstParameterDecl($1); }
	;

initializer :
    assignment_expr { $$ = new AstInitializer($1); }
	/* | '{' initializer_list '}' */
	/* | '{' initializer_list ',' '}' */
	;

stmt :
    /* labeled_stmt
    |*/ compound_stmt { $$ = new AstStmt($1); }
    | expr_stmt { $$ = new AstStmt($1); }
    | selection_stmt { $$ = new AstStmt($1); }
    | jump_stmt { $$ = new AstStmt($1); }
    | iter_stmt
    ;

selection_stmt :
    IF '(' expr ')' stmt { $$ = new AstSelectionStmt($3, $5); }
	| IF '(' expr ')' stmt ELSE stmt { $$ = new AstSelectionStmt($3, $5, $7); }
    ;

iter_stmt :
    WHILE '(' expr ')' stmt { $$ = new AstIterStmt($3, $5); }
	/* | DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement */
	;

compound_stmt : 
    '{' '}' { $$ = new AstCompoundStmt(); }
	| '{' stmt_list '}' { $$ = new AstCompoundStmt($2); }
	| '{' decl_list '}' { $$ = new AstCompoundStmt($2); }
	| '{' decl_list stmt_list '}' { $$ = new AstCompoundStmt($2, $3); }
	;

decl_list :
    decl { $$ = new AstDeclList($1); }
	| decl_list decl { $$ = $1; $$->add_decl($2); }
	;

stmt_list : 
    stmt { $$ = new AstStmtList($1); }
	| stmt_list stmt { $$ = $1; $$->add_stmt($2); }
    ;

expr_stmt :
    ';' { $$ = new AstExprStmt(); }
    | expr ';' { $$ = new AstExprStmt($1); }
    ;

jump_stmt :
    RETURN ';' { $$ = new AstJumpStmt(AstJumpStmt::StmtType::RETURN); }
    | RETURN expr ';' { $$ = new AstJumpStmt($2); }
    | CONTINUE ';' { $$ = new AstJumpStmt(AstJumpStmt::StmtType::CONTINUE); }
    | BREAK ';' { $$ = new AstJumpStmt(AstJumpStmt::StmtType::BREAK); }
    ;

translation_unit : 
    external_decl { $$ = new AstTranslationUnit($1); unit = $$; }
    | translation_unit external_decl { unit = $1; unit->add_exdec($2); }
    ;

external_decl :
    decl { $$ = new AstExternDecl($1);  }
    | function_def { $$ = new AstExternDecl($1); }
    ;

function_def :
    decl_specifiers declarator decl_list compound_stmt { $$ = new AstFunctionDef($1, $2, $3, $4); }
	| decl_specifiers declarator compound_stmt { $$ = new AstFunctionDef($1, $2, $3); }
	| declarator decl_list compound_stmt { $$ = new AstFunctionDef($1, $2, $3); }
	| declarator compound_stmt { $$ = new AstFunctionDef($1, $2); }
	;

/* ------------------------------------------------------------ */ 

%%