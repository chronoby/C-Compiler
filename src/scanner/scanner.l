%{
#include <string>
#include "../ast/ast.h"
#include "../parser/parser.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int yywrap() { }
%}

%%

[ \t\n]                 ;
"int"			        { return(INT); }
[a-zA-Z_][a-zA-Z0-9_]*  SAVE_TOKEN; return IDENTIFIER;
[0-9]+                  SAVE_TOKEN; return INTEGER;
"="                     return('=');
"+"                     return('+');
"-"                     return('-');
";"                     return(';');
.                       printf("Unknown token!n"); yyterminate();

%%
