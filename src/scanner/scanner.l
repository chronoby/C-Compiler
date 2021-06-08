%{
#include <string>
#include "../ast/ast.h"
#include "../parser/parser.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int yywrap() { }
%}

WS          [ \t\r]
NEWLINE     [\n]
TYPE_INT    "int"
IDENTIFIER  [a-zA-Z_][a-zA-Z0-9_]*

ASSIGN      "="
PLUS        "+"
MINUS       "-"
SEMI        ";"
LP          "("
RP          ")"
DOT         "."
PTR_OP      "->"
INC_OP      "++"
DEC_OP      "--"

DIGIT       [0-9]
EXP         [Ee][+-]+{DIGIT}+
INT_S       (u|U|l|L)*
FLOAT_S     (f|F|l|L)
INTEGER     [0-9]+{INT_S}?
HEXI        (0(x|X))({DIGIT}|[A-Fa-f])+{INT_S}?
OCTAL       (0[0-7]+){INT_S}?
FLOAT       (({DIGIT}+{EXP})|({DIGIT}*"."{DIGIT}+({EXP})?)|({DIGIT}+"."{DIGIT}*({EXP})?)){FLOAT_S}?
CHAR        '(\\.|[^\\'])+'
STRING      \"(\\.|[^\\"])*\"

%%
{WS}                        ;
{NEWLINE}                   ;
{TYPE_INT}			        return TYPE_INT;

{IDENTIFIER}                SAVE_TOKEN; return IDENTIFIER;

{INTEGER}                   SAVE_TOKEN; return INTEGER;
{HEXI}                      SAVE_TOKEN; return HEXI;
{OCTAL}                     SAVE_TOKEN; return OCTAL;
{FLOAT}                     SAVE_TOKEN; return FLOAT;
{CHAR}                      SAVE_TOKEN; return CHAR;
{STRING}                    SAVE_TOKEN; return STRING;

{ASSIGN}                    return('=');
{PLUS}                      return('+');
{MINUS}                     return('-');
{SEMI}                      return(';');
{LP}                        return('(');
{RP}                        {return(')');}
{DOT}                       return '.';

{PTR_OP}                    return PTR_OP;
{INC_OP}                    return INC_OP;
{DEC_OP}                    return DEC_OP;

.                           printf("Unknown token!n"); yyterminate();

%%
