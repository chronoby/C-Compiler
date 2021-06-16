%option yylineno
%{
#include <string>
#include "../ast/ast.h"
#include "../parser/parser.hpp"
int yycolumn = 1;
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
                       yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + yyleng - 1; \
                       yycolumn += yyleng;

#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int yywrap() { }
%}

%s COMMENT
%s COMMENT2

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
CONTINUE    "continue"
BREAK       "break"
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

<INITIAL>"/*"               {BEGIN COMMENT;}
<COMMENT>"*/"               {BEGIN INITIAL;}
<COMMENT>.                  {;}
<COMMENT>\n                 {yycolumn = 1;}
<INITIAL>"//"               {BEGIN COMMENT2;}
<COMMENT2>\n                {yycolumn = 1; BEGIN INITIAL;}
<COMMENT2>.                 {;}

<INITIAL>{WS}                        ;
<INITIAL>{NEWLINE}                   yycolumn = 1;
<INITIAL>{VOID}                      SAVE_TOKEN; return VOID;
<INITIAL>{TYPE_INT}			         SAVE_TOKEN; return TYPE_INT;
<INITIAL>{TYPE_LONG}                 SAVE_TOKEN; return TYPE_LONG;
<INITIAL>{TYPE_SHORT}                SAVE_TOKEN; return TYPE_SHORT;
<INITIAL>{TYPE_DOUBLE}               SAVE_TOKEN; return TYPE_DOUBLE;
<INITIAL>{TYPE_FLOAT}                SAVE_TOKEN; return TYPE_FLOAT;
<INITIAL>{TYPE_SIGNED}               SAVE_TOKEN; return TYPE_SIGNED;
<INITIAL>{TYPE_UNSIGNED}             SAVE_TOKEN; return TYPE_UNSIGNED;
<INITIAL>{TYPE_CHAR}                 SAVE_TOKEN; return TYPE_CHAR;
<INITIAL>{IF}                        SAVE_TOKEN; return IF;
<INITIAL>{ELSE}                      SAVE_TOKEN; return ELSE;
<INITIAL>{WHILE}                     SAVE_TOKEN; return WHILE;
<INITIAL>{CONTINUE}                  SAVE_TOKEN; return CONTINUE;
<INITIAL>{BREAK}                     SAVE_TOKEN; return BREAK;
<INITIAL>{SIZEOF}                    SAVE_TOKEN; return SIZEOF;
<INITIAL>{RETURN}                    SAVE_TOKEN; return RETURN;

<INITIAL>{IDENTIFIER}                SAVE_TOKEN; return IDENTIFIER;

<INITIAL>{INTEGER}                   SAVE_TOKEN; return INTEGER;
<INITIAL>{HEXI}                      SAVE_TOKEN; return HEXI;
<INITIAL>{OCTAL}                     SAVE_TOKEN; return OCTAL;
<INITIAL>{FLOAT}                     SAVE_TOKEN; return FLOAT;
<INITIAL>{CHAR}                      SAVE_TOKEN; return CHAR;

<INITIAL>{STRING}                    SAVE_TOKEN; return STRING;

<INITIAL>{PTR_OP}                    return PTR_OP;
<INITIAL>{INC_OP}                    return INC_OP;
<INITIAL>{DEC_OP}                    return DEC_OP;
<INITIAL>{AND_OP}                    return AND_OP;
<INITIAL>{OR_OP}                     return OR_OP;
<INITIAL>{EQ_OP}                     return EQ_OP;
<INITIAL>{NE_OP}                     return NE_OP;
<INITIAL>{LE_OP}                     return LE_OP;
<INITIAL>{GE_OP}                     return GE_OP;

<INITIAL>{AND}                       return('&');
<INITIAL>{ASSIGN}                    return('=');
<INITIAL>{PLUS}                      return('+');
<INITIAL>{MINUS}                     return('-');
<INITIAL>{MULTI}                     return('*');
<INITIAL>{DIV}                       return('/');
<INITIAL>{MOD}                       return('%');
<INITIAL>{SEMI}                      return(';');
<INITIAL>{LP}                        return('(');
<INITIAL>{RP}                        {return(')');}
<INITIAL>{LB}                        return('{');
<INITIAL>{RB}                        return('}');
<INITIAL>{LF}                        return('[');
<INITIAL>{RF}                        return(']');
<INITIAL>{DOT}                       return '.';
<INITIAL>{COMMA}                     return ',';
<INITIAL>{LESS}                      return '<';
<INITIAL>{GREATER}                   return '>';
<INITIAL>{ELLIPSIS}                  return ELLIPSIS;

<INITIAL>.                           printf("Unknown token!n"); yyterminate();

%%
