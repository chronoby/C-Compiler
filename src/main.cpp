#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "visitor/visitor.h"

extern int yyparse();
extern "C" FILE *yyin;
extern AstTranslationUnit* unit;

void invalid_input_err()
{
    std::cerr << "compiler:\033[31m fatal error:\033[0m no input file" << std::endl;
    std::cerr << "terminated" << std::endl;
}

void help_msg()
{
    std::cout << "Usage: compiler <FILE> [options]" << std::endl;
    std::cout << "Options:" << std::endl
              << " -l <FILE_NAME>: write llvm IR code into <FILE_NAME>" << std::endl
              << " -s <FILE_NAME>: write assembly code into <FILE_NAME>" << std::endl
              << " -o <FILE_NAME>: generate execuable code with name <FILE_NAME>" << std::endl;
}

int main(int argc, char **argv)
{
    int input_status = 1; // 0 for ok, 1 for error
    
    bool ir = false;
    bool asm_ = false;
    bool obj = false;

    char* ir_filename = "a.ll";
    char* asm_filename = "a.s";
    char* obj_filename = "a.out";
    if (argc > 1)
    {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
        {
            help_msg();
            return 0;
        }

        const char *path = argv[1];

        FILE *pt = fopen(path, "r");
        if (pt)
        {
            for (int i = 2; i < argc; i++)
            {
                if (!strcmp(argv[i], "-l"))
                {
                    ir = true;
                    if (i + 1 < argc && strcmp(argv[i + 1], "-l") && strcmp(argv[i + 1], "-s") && strcmp(argv[i + 1], "-o")) ir_filename = argv[i + 1];
                }
                else if (!strcmp(argv[i], "-s"))
                {
                    asm_ = true;
                    if (i + 1 < argc && strcmp(argv[i + 1], "-l") && strcmp(argv[i + 1], "-s") && strcmp(argv[i + 1], "-o")) asm_filename = argv[i + 1];
                }   
                else if (!strcmp(argv[i], "-o"))
                {
                    obj = true;
                    if (i + 1 < argc && strcmp(argv[i + 1], "-l") && strcmp(argv[i + 1], "-s") && strcmp(argv[i + 1], "-o")) obj_filename = argv[i + 1];
                }
            }

            if (!(ir || asm_ || obj)) ir = true;

            input_status = 0;
            yyin = pt;
            yyparse();

            Visitor codeGenerator;
            codeGenerator.codegenProgram(unit, ir_filename);

            if (asm_ || obj)
            {
                std::string inst = std::string("llc ") + ir_filename + " -o=" + asm_filename;
                system(inst.c_str());
            }

            if (obj)
            {
                std::string inst = std::string("gcc -no-pie ") + asm_filename + " -o " + obj_filename;
                system(inst.c_str());
            }

            if (!ir) remove(ir_filename);
            if (!asm_) remove(asm_filename);
        }
        else
        {
            std::cerr << "compiler:\033[31m error:\033[0m " << std::string(path) << " is not a valid input file" << std::endl;
        }
    }

    if (input_status)
    {
        invalid_input_err();
        return 1;
    }
    
    return 0;
}