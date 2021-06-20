%option yylineno

%{
#include "syntax.tab.h"
#include <string.h>
#include "Node.h"
int yycolumn =1;
extern int misline[4096];
extern int mistake;

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
NOT         "!"
WAVE		"~"
QUES      "?"
COLON     ":"

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

{WS}                {}
{NEWLINE}           {yycolumn=1;}
{VOID}              {yylval.type_node=create("VOID","",0); return VOID;}
{TYPE_INT}			{yylval.type_node=create("TYPE_INT","",0); return TYPE_INT;}
{TYPE_LONG}         {yylval.type_node=create("TYPE_LONG","",0); return TYPE_LONG;}
{TYPE_SHORT}        {yylval.type_node=create("TYPE_SHORT","",0); return TYPE_SHORT;}
{TYPE_DOUBLE}       {yylval.type_node=create("TYPE_DOUBLE","",0); return TYPE_DOUBLE;}
{TYPE_FLOAT}        {yylval.type_node=create("TYPE_FLOAT","",0); return TYPE_FLOAT;}
{TYPE_SIGNED}       {yylval.type_node=create("TYPE_SIGNED","",0); return TYPE_SIGNED;}
{TYPE_UNSIGNED}     {yylval.type_node=create("TYPE_UNSIGNED","",0); return TYPE_UNSIGNED;}
{TYPE_CHAR}         {yylval.type_node=create("TYPE_CHAR","",0); return TYPE_CHAR;}
{IF}                {yylval.type_node=create("IF","",0); return IF;}
{ELSE}              {yylval.type_node=create("ELSE","",0); return ELSE;}
{WHILE}             {yylval.type_node=create("WHILE","",0); return WHILE;}
{CONTINUE}          {yylval.type_node=create("CONTINUE","",0); return CONTINUE;}
{BREAK}             {yylval.type_node=create("BREAK","",0); return BREAK;}
{SIZEOF}            {yylval.type_node=create("SIZEOF","",0); return SIZEOF;}
{RETURN}            {yylval.type_node=create("RETURN","",0); return RETURN;}

{IDENTIFIER}        {yylval.type_node=create("IDENTIFIER",yytext,0); return IDENTIFIER;}

{INTEGER}           {yylval.type_node=create("INTEGER",yytext,0); return INTEGER;}
{HEXI}              {yylval.type_node=create("HEXI",yytext,0); return HEXI;}
{OCTAL}             {yylval.type_node=create("OCTAL",yytext,0); return OCTAL;}
{FLOAT}             {yylval.type_node=create("FLOAT",yytext,0); return FLOAT;}
{CHAR}              {yylval.type_node=create("CHAR",yytext,0); return CHAR;}

{STRING}            {yylval.type_node=create("STRING",yytext,0); return STRING;}

\/\*    {
            int line=yylineno;
            char c=input();
            int flag=0;
            while(c!=0)
            {
                if(flag==1)
                {
                    if(c=='/'){
                        flag=2;
                        break;
                    }
                    else{
                        if(c!='*')
                            flag=0;
                    }
                }
                else{
                    if(c=='*')
                        flag=1;
                }
                c=input();
            }
            if(flag!=2){
                mistake++;
                printf("Error at Line %d: The  \/\* can not match a \*\/.\n",line);
                misline[line-1]=1;
            }
        }  
"*/" { mistake++;printf("Error at Line %d: syntax error.\n", yylineno);
            misline[yylineno-1]=1;
            }

\/\/ { char c=input();
            while(c!='\n')
            c=input();
            }



{PTR_OP}            {yylval.type_node=create("PTR_OP","",0); return PTR_OP;}
{INC_OP}            {yylval.type_node=create("INC_OP","",0); return INC_OP;}
{DEC_OP}            {yylval.type_node=create("DEC_OP","",0); return DEC_OP;}
{AND_OP}            {yylval.type_node=create("AND_OP","",0); return AND_OP;}
{OR_OP}             {yylval.type_node=create("OR_OP","",0); return OR_OP;}
{EQ_OP}             {yylval.type_node=create("EQ_OP","",0); return EQ_OP;}
{NE_OP}             {yylval.type_node=create("NE_OP","",0); return NE_OP;}
{LE_OP}             {yylval.type_node=create("LE_OP","",0); return LE_OP;}
{GE_OP}             {yylval.type_node=create("GE_OP","",0); return GE_OP;}

{QUES}               {yylval.type_node=create("QUES","",0); return QUES;}
{COLON}               {yylval.type_node=create("COLON","",0); return COLON;}
{NOT}               {yylval.type_node=create("NOT","",0); return NOT;}
{WAVE}               {yylval.type_node=create("WAVE","",0); return WAVE;}
{AND}               {yylval.type_node=create("AND","",0); return AND;}
{ASSIGN}            {yylval.type_node=create("ASSIGN","",0); return ASSIGN;}
{PLUS}              {yylval.type_node=create("PLUS","",0); return PLUS;}
{MINUS}             {yylval.type_node=create("MINUS","",0); return MINUS;}
{MULTI}             {yylval.type_node=create("MULTI","",0); return MULTI;}
{DIV}               {yylval.type_node=create("DIV","",0); return DIV;}
{MOD}               {yylval.type_node=create("MOD","",0); return MOD;}
{SEMI}              {yylval.type_node=create("SEMI","",0); return SEMI;}
{LP}                {yylval.type_node=create("LP","",0); return LP;}
{RP}                {yylval.type_node=create("RP","",0); return RP;}
{LB}                {yylval.type_node=create("LB","",0); return LB;}
{RB}                {yylval.type_node=create("RB","",0); return RB;}
{LF}                {yylval.type_node=create("LF","",0); return LF;}
{RF}                {yylval.type_node=create("RF","",0); return RF;}
{DOT}               {yylval.type_node=create("DOT","",0); return DOT;}
{COMMA}             {yylval.type_node=create("COMMA","",0); return COMMA;}
{LESS}              {yylval.type_node=create("LESS","",0); return LESS;}
{GREATER}           {yylval.type_node=create("GREATER","",0); return GREATER;}
{ELLIPSIS}          {yylval.type_node=create("ELLIPSIS","",0); return ELLIPSIS;}

.                   {
						mistake++;
    					printf("Error at Line %d: Mysterious characters \'%s'\n",yylineno,yytext);
    					misline[yylineno-1]=1;
					}

%%
