#include <iostream>

#include "llvm/IR/PassManager.h"
#include "llvm/Support/Host.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/Instructions.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"


#include "visitor.h"

Visitor::Visitor()
{
    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<> >(*context);
    module = std::make_unique<llvm::Module>("main", *context);
    

    // std::vector<llvm::Type*> argTypes;
    // llvm::FunctionType* ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(*context), argTypes, false);
    // llvm::Function* mainFunction = llvm::Function::Create(ftype, llvm::Function::InternalLinkage, "main", *module);
    // block = llvm::BasicBlock::Create(*context, "entry", mainFunction, 0);

    envs.push_back(new LocalEnv());
}

void Visitor::codegenProgram(AstTranslationUnit* root)
{
    root->codegen(*this);

    std::cout << "Generate code completed" << std::endl << "LLVM IR:" << std::endl;
    std::string llvm_IR;
    llvm::raw_string_ostream OS(llvm_IR);
    OS << *module;
    // std::cout << "get this line "<< std::endl;
    OS.flush();
    std::cout << llvm_IR;

    // llvm::PassManager pm;
    // pm.add(createPrintModulePass(&outs()));
    // pm.run(*module);
}

void Visitor::configureTarget()
{
    auto target = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(target);
}

// --------------------- EXPRESSION -----------------------------

llvm::Value* Visitor::codegen(const AstPrimaryExpr& node)
{
    // TO BE FINISHED
    std::cout << "Creating primary expr" << node.value << std::endl;
    switch (node.expr_type)
    {
    case AstPrimaryExpr::ExprType::CONSTANT:
    {
        switch (node.data_type)
        {
        case AstPrimaryExpr::DataType::INTEGER:
        {
            int val = strtol(node.value.c_str(), nullptr, 10);
            return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), val, true);
            break;
        }
        case AstPrimaryExpr::DataType::OCTAL:
        {
            int val = strtol(node.value.c_str(), nullptr, 8);
            return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), val, true);
            break;
        }
        case AstPrimaryExpr::DataType::HEXI:
        {
            int val = strtol(node.value.c_str(), nullptr, 16);
            return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), val, true);
            break;
        }
        case AstPrimaryExpr::DataType::FLOAT:
        {
            double val = strtod(node.value.c_str(), nullptr);
            return llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context), val);
            break;
        }
        case AstPrimaryExpr::DataType::CHAR:
        {
            if (node.value.length() == 0)
            {
                return llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), 0, true);
            }
            else
            {
                char val = node.value[1];
                return llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), val, true);
            }
            break;
        }
        case AstPrimaryExpr::DataType::STRING:
        {
            std::string str_content = node.value.substr(1, node.value.length() - 2);
            int content_len = str_content.length();
            if (content_len > 255) {
                std::cerr << "WARNING: CUT string literal to length of 255" << std::endl;
                str_content = str_content.substr(0, 255);
                content_len = str_content.size();
            }
            // char zero = 0;
            // for (int i = 0; i < 255 - content_len; i++) str_content = str_content + zero;

            // std::cout << str_content << std::endl;

            llvm::Value* str_mem = this->builder->CreateGlobalStringPtr(str_content, "", 0, &*this->module);
            // llvm::Value* str_load = this->builder->CreateLoad(str_mem);
            // std::cout << str_mem << std::endl;
            return str_mem;
            break;
        }
        default:
        {
            std::cerr << "ERROR: Invalid Datatype" << std::endl;
            return nullptr;
        }
        }
    }
    case AstPrimaryExpr::ExprType::ID:
    {
        for (int i = this->envs.size() - 1; i >= 0; i--)
        {
            auto env = this->envs[i];
            auto pair = env->locals.find(node.identifier_name);
            if (pair != env->locals.end())
            {
                auto var = pair->second;
                return this->builder->CreateLoad(var);
            }
            if (i == 0)
            {
                std::cerr << "ERROR: identifier not defined: " << node.identifier_name << std::endl;
                return nullptr;
            }
        }
    }
    case AstPrimaryExpr::ExprType::PR_EXPR:
    {
        return node.expr->codegen(*this);
    }
    default:
    {
        std::cerr << "ERROR: INVALID AstPrimaryExpr node" << std::endl;
        return nullptr;
    }
    }
}

llvm::Value* Visitor::codegen(const AstPostfixExpr& node)
{
    switch (node.expr_type)
    {
        case AstPostfixExpr::ExprType::PRIMARY:
        {
            return node.primary_expr->codegen(*this);
        }
        // case AstPostfixExpr::ExprType::IDX:
        // {
        // }
        // case AstPostfixExpr::ExprType::OP:
        // {
        //     return node.primary_expr->codegen(*this);
        // }
        case AstPostfixExpr::ExprType::FUNC:
        {
            llvm::Function* func = nullptr;
            for (int i = this->envs.size() - 1; i >= 0; i--)
            {
                auto env = this->envs[i];
                auto pair = env->functions.find(node.identifier_name);
                if (pair != env->functions.end())
                {
                    func = pair->second;
                    break;
                }
                if (i == 0)
                {
                    std::cerr << "ERROR: function not found: "<< node.identifier_name << std::endl;
                    return nullptr;
                }
            }
            // std::cerr << "func: " << func << std::endl;
            return this->builder->CreateCall(func);
        }
        case AstPostfixExpr::ExprType::FUNC_PARAM:
        {
            llvm::Function* func = nullptr;
            llvm::FunctionType* func_type = nullptr;
            for (int i = this->envs.size() - 1; i >= 0; i--)
            {
                auto env = this->envs[i];
                auto pair = env->functions.find(node.identifier_name);
                if (pair != env->functions.end())
                {
                    func = pair->second;
                    auto pair_type = env->function_types.find(node.identifier_name);
                    func_type = pair_type->second;
                    break;
                }
                if (i == 0)
                {
                    std::cerr << "ERROR: function not found: "<< node.identifier_name << std::endl;
                    return nullptr;
                }
            }
            std::vector<llvm::Value*> args;
            auto expr_list = node.argument_expr_list;
            if (expr_list)
            {
                for (auto expr : expr_list->expr_list)
                {
                    args.push_back(expr->codegen(*this));
                }
            }
            return this->builder->CreateCall(func, args);
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstUnaryExpr& node)
{
    switch (node.expr_type)
    {
        case AstUnaryExpr::ExprType::POSTFIX:
        {
            return node.postfix_expr->codegen(*this);
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstCastExpr& node)
{
    switch (node.expr_type)
    {
        case AstCastExpr::ExprType::UNARY:
        {
            return node.unary_expr->codegen(*this);
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstMultiplicativeExpr& node)
{
    switch (node.expr_type)
    {
        case AstMultiplicativeExpr::ExprType::CAST:
        {
            return node.cast_expr->codegen(*this);
        }
        case AstMultiplicativeExpr::ExprType::OP:
        {
            switch (node.op_type)
            {
                case AstMultiplicativeExpr::OpType::MUL:
                {
                    return this->builder->CreateMul(node.multi_expr->codegen(*this), node.cast_expr->codegen(*this), "mul");
                }
                case AstMultiplicativeExpr::OpType::DIV:
                {
                    // only for signed int
                    // to support double, add CreateFDiv. but how to get type?
                    return this->builder->CreateSDiv(node.multi_expr->codegen(*this), node.cast_expr->codegen(*this), "div");
                }
                case AstMultiplicativeExpr::OpType::MOD:
                {
                    return this->builder->CreateSRem(node.multi_expr->codegen(*this), node.cast_expr->codegen(*this), "ram");
                }
            }
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstAdditiveExpr& node)
{
    switch (node.expr_type)
    {
        case AstAdditiveExpr::ExprType::MULTI:
        {
            return node.multi_expr->codegen(*this);
        }
        case AstAdditiveExpr::ExprType::OP:
        {
            switch (node.op_type)
            {
                case AstAdditiveExpr::OpType::PLUS:
                {
                    return this->builder->CreateAdd(node.add_expr->codegen(*this), node.multi_expr->codegen(*this), "add");
                }
                case AstAdditiveExpr::OpType::MINUS:
                {
                    return this->builder->CreateSub(node.add_expr->codegen(*this), node.multi_expr->codegen(*this), "sub");
                }
            }
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstShiftExpr& node)
{
    switch (node.expr_type)
    {
        case AstShiftExpr::ExprType::ADD:
        {
            return node.add_expr->codegen(*this);
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstRelationalExpr& node)
{
    switch (node.expr_type)
    {
        case AstRelationalExpr::ExprType::SHIFT:
        {
            return node.shift_expr->codegen(*this);
        }
        case AstRelationalExpr::ExprType::OP:
        {
            switch (node.op_type)
            {
                case AstRelationalExpr::OpType::GREATER:
                {
                    return builder->CreateICmpSGT(node.rela_expr->codegen(*this), node.shift_expr->codegen(*this), "gt");
                }
                case AstRelationalExpr::OpType::LESS:
                {
                    return builder->CreateICmpSLT(node.rela_expr->codegen(*this), node.shift_expr->codegen(*this), "lt");
                }
                case AstRelationalExpr::OpType::GE:
                {
                    return builder->CreateICmpSGE(node.rela_expr->codegen(*this), node.shift_expr->codegen(*this), "gte");
                }
                case AstRelationalExpr::OpType::LE:
                {
                    return builder->CreateICmpSLE(node.rela_expr->codegen(*this), node.shift_expr->codegen(*this), "lte");
                }
            }
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstEqualityExpr& node)
{
    switch (node.expr_type)
    {
        case AstEqualityExpr::ExprType::RELATIONAL:
        {
            return node.rela_expr->codegen(*this);
        }
        case AstEqualityExpr::ExprType::OP:
        {
            switch (node.op_type)
            {
                case AstEqualityExpr::OpType::EQ:
                {
                    return builder->CreateICmpEQ(node.equal_expr->codegen(*this), node.rela_expr->codegen(*this), "eq");
                }            
                case AstEqualityExpr::OpType::NE:
                {
                    return builder->CreateICmpNE(node.equal_expr->codegen(*this), node.rela_expr->codegen(*this), "ne");
                }
            }
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstAndExpr& node)
{
    switch (node.expr_type)
    {
        case AstAndExpr::ExprType::EQUALITY:
        {
            return node.equal_expr->codegen(*this);
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstExclusiveExpr& node)
{
    switch (node.expr_type)
    {
        case AstExclusiveExpr::ExprType::AND:
        {
            return node.and_expr->codegen(*this);
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstInclusiveExpr& node)
{
    switch (node.expr_type)
    {
        case AstInclusiveExpr::ExprType::EXCLUSIVE:
        {
            return node.exclusive_expr->codegen(*this);
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstLogicalAndExpr& node)
{
    switch (node.expr_type)
    {
        case AstLogicalAndExpr::ExprType::INCLUSIVE:
        {
            return node.inclusive_expr->codegen(*this);
        }
        case AstLogicalAndExpr::ExprType::OP:
        {
            return builder->CreateLogicalAnd(node.and_expr->codegen(*this), node.inclusive_expr->codegen(*this), "logical_and");
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstLogicalOrExpr& node)
{
    switch (node.expr_type)
    {
        case AstLogicalOrExpr::ExprType::LOG_AND:
        {
            return node.and_expr->codegen(*this);
        }
        case AstLogicalOrExpr::ExprType::OP:
        {
            return builder->CreateLogicalOr(node.or_expr->codegen(*this), node.and_expr->codegen(*this), "logical_or");
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstConditionalExpr& node)
{
    switch (node.expr_type)
    {
        case AstConditionalExpr::ExprType::LOG_OR:
        {
            return node.or_expr->codegen(*this);
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstAssignmentExpr& node)
{
    switch (node.expr_type)
    {
        case AstAssignmentExpr::ExprType::CONDITIONAL:
        {
            return node.cond_expr->codegen(*this);
        }
        case AstAssignmentExpr::ExprType::ASSIGN:
        {
            // need update
            // return new llvm::StoreInst(node.assign_expr->codegen(*this), node.unary_expr->codegen(*this), false, block);
            return this->builder->CreateStore(node.assign_expr->codegen(*this), node.unary_expr->codegen(*this), false);
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstExpr& node)
{
    switch (node.expr_type)
    {
        case AstExpr::ExprType::ASSIGN:
        {
            return node.assign_expr->codegen(*this);
        }
    }
    return nullptr;
}

// ---------------- declaration ----------------------------

llvm::Type* Visitor::codegen(const AstTypeSpecifier& node)
{
    switch (node.type)
    {
        case (AstTypeSpecifier::Type::INT):
        {
            return llvm::Type::getInt32Ty(*context);
        }
        case (AstTypeSpecifier::Type::SHORT):
        {
            return llvm::Type::getInt16Ty(*context);
        }
        case (AstTypeSpecifier::Type::LONG):
        {
            return llvm::Type::getInt32Ty(*context);
        }
        case (AstTypeSpecifier::Type::DOUBLE):
        {
            return llvm::Type::getDoubleTy(*context);
        }
        case (AstTypeSpecifier::Type::CHAR):
        {
            return llvm::Type::getInt8Ty(*context);            
        }
        case (AstTypeSpecifier::Type::FLOAT):
        {
            return llvm::Type::getFloatTy(*context);
        }
        default:
        {
            std::cerr << "ERROR: invalid data type" << std::endl;
            return nullptr;
        }
    }
}

llvm::Value* Visitor::codegen(const AstDecl& node)
{
    if (!node.decl_specifiers || node.decl_specifiers->type_specs.size() != 1)
    {
        std::cerr << "ERROR: invalid declaration specifiers" << std::endl;
        return nullptr;
    }
    auto type_spec = node.decl_specifiers->type_specs[0];
    if (!node.init_declarator_list)
    {
        std::cerr << "ERROR: invalid init declarators" << std::endl;
        return nullptr;
    }
    auto init_declarators = node.init_declarator_list->init_declarators;
    for (auto init_declarator : init_declarators)
    {
        auto declarator = init_declarator->declarator;
        auto initializer = init_declarator->initializer;

        if (declarator->declarator_type == AstDeclarator::DeclaratorType::VAR)
        {
            std::string var_name = declarator->direct_declarator->id_name;
            auto var_type = type_spec->codegen(*this);
            llvm::Constant* initializer_v = llvm::ConstantAggregateZero::get(var_type);

            if (envs.size() == 1)
            {
                if (initializer)
                {
                    auto initializer_value = initializer->codegen(*this);
                    if (llvm::isa<llvm::Constant>(initializer_value))
                    {
                        initializer_v = llvm::cast<llvm::Constant>(initializer_value);
                    }
                    else
                    {
                        std::cerr << "ERROR: global variables must be initialized with constants" << std::endl;
                        std::cerr << "initialize global variable " << var_name << "with zero" << std::endl; 
                    }
                }

                LocalEnv* present_env = envs[0];
                if (present_env->locals.find(var_name) != present_env->locals.end())
                {
                    std::cerr << "ERROR: variable redeclaration: " << var_name << std::endl; 
                    return nullptr;
                }

                llvm::GlobalVariable *var = new llvm::GlobalVariable(
                    *(this->module),
                    var_type,
                    false,
                    llvm::GlobalValue::CommonLinkage,
                    initializer_v,
                    var_name
                );
                
                present_env->locals.insert({var_name, var});
                return var;
            }
            else
            {
                LocalEnv* present_env = envs.back();
                if (present_env->locals.find(var_name) != present_env->locals.end())
                {
                    std::cerr << "ERROR: variable redeclaration: " << var_name << std::endl; 
                    return nullptr;
                }

                llvm::AllocaInst* var = this->builder->CreateAlloca(var_type, nullptr, var_name);

                if (initializer)
                {
                    auto initializer_value = initializer->codegen(*this);
                    llvm::Value* store_inst = this->builder->CreateStore(initializer_value, var, false);
                }

                present_env->locals.insert({var_name, var});
            }
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstInitializer& node)
{
    return node.assignment_expr->codegen(*this);
}

llvm::Value* Visitor::codegen(const AstStmt& node)
{
    switch (node.stmt_type)
    {
    case AstStmt::StmtType::EXPR :
        return node.expr_stmt->codegen(*this);
        break;
    
    default:
        break;
    }
}

llvm::Value* Visitor::codegen(const AstCompoundStmt& node)
{
    auto env = new LocalEnv();
    this->envs.push_back(env);
    
    if (node.decl_list)
    {
        node.decl_list->codegen(*this);
    }

    if (node.stmt_list)
    {
        node.stmt_list->codegen(*this);
    }

    this->envs.pop_back();
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstDeclList& node)
{
    for (auto decl : node.decls)
    {
        if (decl)
        {
            decl->codegen(*this);
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstStmtList& node)
{
    for (auto stmt : node.stmts)
    {
        if (stmt)
        {
            stmt->codegen(*this);
        }
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstTranslationUnit& node)
{
    for (auto i : node.external_decl_list)
    {
        i->codegen(*this);
    }
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstExternDecl& node)
{
    switch(node.decl_type)
    {
        case (AstExternDecl::DeclType::VAR):
        {
            return node.declaration->codegen(*this);
        }
        case (AstExternDecl::DeclType::FUNC):
        {
            return node.function_definition->codegen(*this);
        }
    }
}

llvm::Value* Visitor::codegen(const AstFunctionDef& node)
{
    llvm::Type* result_type = llvm::Type::getInt32Ty(*context);
    auto decl_specs = node.decl_specifiers;
    if (decl_specs && decl_specs->type_specs.size() > 0)
    {  
        auto type_specs = decl_specs->type_specs[0];
        result_type = type_specs->codegen(*this);
    }
    
    auto direct_declarator = node.declarator->direct_declarator;
    llvm::FunctionType* func_type;
    switch(direct_declarator->declarator_type)
    {
        case AstDirectDeclarator::DeclaratorType::FUNC_EMPTY:
        {
            func_type = llvm::FunctionType::get(result_type, false);
            break;
        }
        case AstDirectDeclarator::DeclaratorType::FUNC_PARAM:
        {
            std::vector<llvm::Type*> param_types;
            bool isVarArg = direct_declarator->param_type_list->isVarArg;
            auto parameter_list_node = direct_declarator->param_type_list->param_list;
            for (auto param_decl : parameter_list_node->parameter_list)
            {
                auto type_spec = param_decl->decl_specifiers->type_specs[0];
                auto type = type_spec->codegen(*this);
                param_types.push_back(type);
            }
            func_type = llvm::FunctionType::get(result_type, param_types, isVarArg);
            break;
        }
    }
    
    llvm::Function* function = llvm::Function::Create(func_type, llvm::GlobalValue::ExternalLinkage, direct_declarator->id_name, &*this->module);
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context, "entry", function, nullptr);
    this->builder->SetInsertPoint(entry);

    node.compound_stmt->codegen(*this);
    this->envs.back()->functions.insert({direct_declarator->id_name, function});
    this->envs.back()->function_types.insert({direct_declarator->id_name, func_type});
    return function;
}

llvm::Value* Visitor::codegen(const AstExprStmt& node)
{
    return node.expr->codegen(*this);
}

// ------------------------------------------------------------------------------

