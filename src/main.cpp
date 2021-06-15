#include <iostream>
#include "visitor/visitor.h"

extern int yyparse();
extern "C" FILE *yyin;
extern AstPrimaryExpr* primary;
extern AstExpr* expre;
extern AstTranslationUnit* unit;

int main(int argc, char **argv)
{
    std::cout << "Begin parsing" << std::endl;

    const char *path = "test/test2.c";
    FILE *pt = fopen(path, "r");
    if(!pt)
    {
        std::cout << "Input file not exist" << std::endl;
        return -1;
    }
    yyin = pt;
    yyparse();

    Visitor codeGenerator;
    std::cout << "Begin generating code" << std::endl;
    codeGenerator.codegenProgram(unit);
    // codeGenerator.configureTarget();
    
    return 0;
}