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
TYPE_CHAR   "char"
TYPE_FLOAT  "float"
TYPE_DOUBLE "double"
TYPE_LONG   "long"
TYPE_SHORT  "short"
TYPE_SIGNED "signed"
TYPE_UNSIGNED   "unsigned"
VOID        "void"
SIZEOF      "sizeof"
RETURN      "return"
IF          "if"
ELSE        "else"
WHILE       "while"
IDENTIFIER  [a-zA-Z_][a-zA-Z0-9_]*

ASSIGN      "="
PLUS        "+"
MINUS       "-"
MULTI       "*"
DIV         "/"
MOD         "%"
SEMI        ";"
LP          "("
RP          ")"
LB          "{"
RB          "}"
LF          "["
RF          "]"
DOT         "."
COMMA       ","
LESS        "<"
GREATER     ">"
PTR_OP      "->"
INC_OP      "++"
DEC_OP      "--"
AND_OP      "&&"
AND         "&"
OR_OP       "||"
EQ_OP       "=="
NE_OP       "!="
LE_OP       "<="
GE_OP       ">="
ELLIPSIS    "..."

DIGIT       [0-9]
EXP         [Ee][+-]+{DIGIT}+
INT_S       (u|U|l|L)*
FLOAT_S     (f|F|l|L)
INTEGER     (0|[1-9][0-9]*){INT_S}?
HEXI        (0(x|X))({DIGIT}|[A-Fa-f])+{INT_S}?
OCTAL       (0[0-7]+){INT_S}?
FLOAT       (({DIGIT}+{EXP})|({DIGIT}*"."{DIGIT}+({EXP})?)|({DIGIT}+"."{DIGIT}*({EXP})?)){FLOAT_S}?
CHAR        '(\\.|[^\\'])+'
STRING      \"(\\.|[^\\"])*\"

%%
{WS}                        ;
{NEWLINE}                   ;
{VOID}                      SAVE_TOKEN; return VOID;
{TYPE_INT}			        SAVE_TOKEN; return TYPE_INT;
{TYPE_LONG}                 SAVE_TOKEN; return TYPE_LONG;
{TYPE_SHORT}                SAVE_TOKEN; return TYPE_SHORT;
{TYPE_DOUBLE}               SAVE_TOKEN; return TYPE_DOUBLE;
{TYPE_FLOAT}                SAVE_TOKEN; return TYPE_FLOAT;
{TYPE_SIGNED}               SAVE_TOKEN; return TYPE_SIGNED;
{TYPE_UNSIGNED}             SAVE_TOKEN; return TYPE_UNSIGNED;
{TYPE_CHAR}                 SAVE_TOKEN; return TYPE_CHAR;
{IF}                        SAVE_TOKEN; return IF;
{ELSE}                      SAVE_TOKEN; return ELSE;
{WHILE}                     SAVE_TOKEN; return WHILE;
{SIZEOF}                    SAVE_TOKEN; return SIZEOF;
{RETURN}                    SAVE_TOKEN; return RETURN;

{IDENTIFIER}                SAVE_TOKEN; return IDENTIFIER;

{INTEGER}                   SAVE_TOKEN; return INTEGER;
{HEXI}                      SAVE_TOKEN; return HEXI;
{OCTAL}                     SAVE_TOKEN; return OCTAL;
{FLOAT}                     SAVE_TOKEN; return FLOAT;
{CHAR}                      SAVE_TOKEN; return CHAR;

{STRING}                    SAVE_TOKEN; return STRING;

{PTR_OP}                    return PTR_OP;
{INC_OP}                    return INC_OP;
{DEC_OP}                    return DEC_OP;
{AND_OP}                    return AND_OP;
{OR_OP}                     return OR_OP;
{EQ_OP}                     return EQ_OP;
{NE_OP}                     return NE_OP;
{LE_OP}                     return LE_OP;
{GE_OP}                     return GE_OP;

{AND}                       return('&');
{ASSIGN}                    return('=');
{PLUS}                      return('+');
{MINUS}                     return('-');
{MULTI}                     return('*');
{DIV}                       return('/');
{MOD}                       return('%');
{SEMI}                      return(';');
{LP}                        return('(');
{RP}                        {return(')');}
{LB}                        return('{');
{RB}                        return('}');
{LF}                        return('[');
{RF}                        return(']');
{DOT}                       return '.';
{COMMA}                     return ',';
{LESS}                      return '<';
{GREATER}                   return '>';
{ELLIPSIS}                  return ELLIPSIS;

.                           printf("Unknown token!n"); yyterminate();

%%
