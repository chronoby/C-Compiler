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
    

    std::vector<llvm::Type*> argTypes;
    llvm::FunctionType* ftype = llvm::FunctionType::get(llvm::Type::getVoidTy(*context), argTypes, false);
    llvm::Function* mainFunction = llvm::Function::Create(ftype, llvm::Function::InternalLinkage, "main", *module);
    block = llvm::BasicBlock::Create(*context, "entry", mainFunction, 0);
}

void Visitor::codegenProgram(AstPrimaryExpr* root)
{
    root->codegen(*this);

    std::cout << "Generate code completed" << std::endl << "LLVM IR:" << std::endl;
    std::string llvm_IR;
    llvm::raw_string_ostream OS(llvm_IR);
    OS << *module;
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

llvm::Value* Visitor::codegen(const AstPrimaryExpr& node)
{
    // TO BE FINISHED
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
            
            return nullptr;
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
        // NOTE: to be updated when the variable mapping mechanism updates
        auto local_pair = this->locals.find(node.identifier_name);
        if (local_pair != this->locals.end())
        {
            return new llvm::LoadInst(local_pair->second->getType(), local_pair->second, node.identifier_name, false, block);
        }
        else
        {
            std::cerr << "ERROR: identifier not defined: " << node.identifier_name << std::endl;
            return nullptr;
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

llvm::Value* Visitor::codegen(const AstProfixExpr& node)
{
    // TO BE FINISHED
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstExpr& node)
{
    // TO BE FINISHED
    return nullptr;
}

llvm::Value* Visitor::codegen(const AstInt& node)
{
    std::cout << "Creating int" << std::endl;
    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), node.val, true);
}

llvm::Value* Visitor::codegen(const AstIdentifier& node)
{
    std::cout << "Creating id" << std::endl;
    std::string name = node.getName();
    if(locals.find(name) == locals.end())
    {
        std::cout << "Undefined variable: " << name << std::endl;
        return nullptr;
    }
    return new llvm::LoadInst(llvm::Type::getInt32Ty(*context), locals[name], "", false, block);
}

llvm::Value* Visitor::codegen(const AstExprStmt& node)
{
    std::cout << "Creating expression statement" << std::endl;
    return node.getExpr()->codegen(*this);
}

llvm::Value* Visitor::codegen(const AstAssignment& node)
{
    std::cout << "Creating assignment" << std::endl;
    std::string name = node.getName();
    if(locals.find(name) == locals.end())
    {
        std::cout << "Undefined variable: " << name << std::endl;
        return nullptr;
    }
    return new llvm::StoreInst(node.getExpr()->codegen(*this), locals[name], false, block);
}

llvm::Value* Visitor::codegen(AstVariableDeclaration& node)
{
    std::cout << "Creating variable declaration" << std::endl;
    
    // llvm::AllocaInst* alloc = new llvm::AllocaInst(llvm::Type::getInt32Ty(*context), llvm::getInt32(1), node.id->name.c_str(), block);
    llvm::AllocaInst* alloc = new llvm::AllocaInst(llvm::Type::getInt32Ty(*context), 0, node.id->name.c_str(), block);
    // llvm::AllocaInst* alloc = builder->CreateAlloca(llvm::Type::getInt32Ty(*context), nullptr, node.id->name.c_str());
    locals[node.getId()->getName()] = alloc;
    return alloc;
}

llvm::Value* Visitor::codegen(const AstBlock& node)
{
    std::cout << "Creating block" << std::endl;
    llvm::Value* last = nullptr;
    // std::cout << node.getStmtList().size() << std::endl;
    // for(auto it = node.getStmtList().begin(); it != node.getStmtList().end(); ++it)
    // {
    //     last = (*it)->codegen(*this);
    // }
    for(auto it = node.stmt_list.begin(); it != node.stmt_list.end(); ++it)
    {
        last = (*it)->codegen(*this);
    }

    return last;
}
