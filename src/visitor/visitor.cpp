#include <iostream>
#include <fstream>

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
    present_function = nullptr;
    func_params = nullptr;
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

    std::ofstream outfile("out.ll");
    outfile << llvm_IR;
    outfile.close();

    // llvm::PassManager pm;
    // pm.add(createPrintModulePass(&outs()));
    // pm.run(*module);
}

void Visitor::configureTarget()
{
    auto target = llvm::sys::getDefaultTargetTriple();
    module->setTargetTriple(target);
}

int Visitor::getTmpVarId()
{
    return tmp_var_id++;
}

// --------------------- EXPRESSION -----------------------------

std::shared_ptr<Variable> Visitor::codegen(const AstPrimaryExpr& node)
{
    // TO BE FINISHED
    std::cout << "Creating primary expr" << node.value << std::endl;
    llvm::Value* value = nullptr;
    llvm::Value* addr = nullptr;
    
    switch (node.expr_type)
    {
    case AstPrimaryExpr::ExprType::CONSTANT:
    {
        switch (node.data_type)
        {
        case AstPrimaryExpr::DataType::INTEGER:
        {
            int val = strtol(node.value.c_str(), nullptr, 10);
            value = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), val, true);
            break;
        }
        case AstPrimaryExpr::DataType::OCTAL:
        {
            int val = strtol(node.value.c_str(), nullptr, 8);
            value = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), val, true);
            break;
        }
        case AstPrimaryExpr::DataType::HEXI:
        {
            int val = strtol(node.value.c_str(), nullptr, 16);
            value = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), val, true);
            break;
        }
        case AstPrimaryExpr::DataType::FLOAT:
        {
            double val = strtod(node.value.c_str(), nullptr);
            value = llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context), val);
            break;
        }
        case AstPrimaryExpr::DataType::CHAR:
        {
            if (node.value.length() == 0)
            {
                value = llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), 0, true);
            }
            else
            {
                char val = node.value[1];
                value = llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), val, true);
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
            addr =  str_mem;
            break;
        }
        default:
        {
            std::cerr << "ERROR: Invalid Datatype" << std::endl;
            return nullptr;
        }
        }
        return std::make_shared<Variable>(value, addr);
    }
    case AstPrimaryExpr::ExprType::ID:
    {
        if (this->present_function)
        {
            auto param_id_pair = func_params->find(node.identifier_name);
            if (param_id_pair != func_params->end())
            {
                auto id = param_id_pair->second;
                auto arg = present_function->getArg(id);
                value = arg;
                return std::make_shared<Variable>(value, addr);
                // return this->builder->CreateLoad(arg->getType(), arg);
            }
        }

        for (int i = this->envs.size() - 1; i >= 0; i--)
        {
            auto env = this->envs[i];
            auto pair = env->locals.find(node.identifier_name);
            if (pair != env->locals.end())
            {
                addr = pair->second;
                value = this->builder->CreateLoad(addr);
                return std::make_shared<Variable>(value, addr);
                // return this->builder->CreateLoad(var);
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

std::shared_ptr<Variable> Visitor::codegen(const AstPostfixExpr& node)
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
            llvm::Value* ret = this->builder->CreateCall(func);
            return std::make_shared<Variable>(ret, nullptr);
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
                    auto arg = expr->codegen(*this)->value;
                    // auto copied_arg = this->builder->CreateAlloca(arg->getType(), nullptr, std::string("tmp_var_") + std::to_string(this->getTmpVarId()));
                    // TODO: add type check
                    args.push_back(arg);
                }
            }
            llvm::Value* ret = this->builder->CreateCall(func, args);
            return std::make_shared<Variable>(ret, nullptr);
        }
    }
    return nullptr;
}

std::shared_ptr<Variable> Visitor::codegen(const AstUnaryExpr& node)
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

std::shared_ptr<Variable> Visitor::codegen(const AstCastExpr& node)
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

std::shared_ptr<Variable> Visitor::codegen(const AstMultiplicativeExpr& node)
{
    switch (node.expr_type)
    {
        case AstMultiplicativeExpr::ExprType::CAST:
        {
            return node.cast_expr->codegen(*this);
        }
        case AstMultiplicativeExpr::ExprType::OP:
        {
            llvm::Value* res;
            switch (node.op_type)
            {
                case AstMultiplicativeExpr::OpType::MUL:
                {
                    res = this->builder->CreateMul(node.multi_expr->codegen(*this)->value, node.cast_expr->codegen(*this)->value, "mul");
                    break;
                }
                case AstMultiplicativeExpr::OpType::DIV:
                {
                    // only for signed int
                    // to support double, add CreateFDiv. but how to get type?
                    res = this->builder->CreateSDiv(node.multi_expr->codegen(*this)->value, node.cast_expr->codegen(*this)->value, "div");
                    break;
                }
                case AstMultiplicativeExpr::OpType::MOD:
                {
                    res = this->builder->CreateSRem(node.multi_expr->codegen(*this)->value, node.cast_expr->codegen(*this)->value, "ram");
                    break;
                }
            }
            return std::make_shared<Variable>(res, nullptr);
        }
    }
    return nullptr;
}

std::shared_ptr<Variable> Visitor::codegen(const AstAdditiveExpr& node)
{
    switch (node.expr_type)
    {
        case AstAdditiveExpr::ExprType::MULTI:
        {
            return node.multi_expr->codegen(*this);
        }
        case AstAdditiveExpr::ExprType::OP:
        {
            llvm::Value* res;
            switch (node.op_type)
            {
                case AstAdditiveExpr::OpType::PLUS:
                {
                    res = this->builder->CreateAdd(node.add_expr->codegen(*this)->value, node.multi_expr->codegen(*this)->value, "add");
                    break;
                }
                case AstAdditiveExpr::OpType::MINUS:
                {
                    res = this->builder->CreateSub(node.add_expr->codegen(*this)->value, node.multi_expr->codegen(*this)->value, "sub");
                    break;
                }
            }
            return std::make_shared<Variable>(res, nullptr);
        }
    }
    return nullptr;
}

std::shared_ptr<Variable> Visitor::codegen(const AstShiftExpr& node)
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

std::shared_ptr<Variable> Visitor::codegen(const AstRelationalExpr& node)
{
    switch (node.expr_type)
    {
        case AstRelationalExpr::ExprType::SHIFT:
        {
            return node.shift_expr->codegen(*this);
        }
        case AstRelationalExpr::ExprType::OP:
        {
            llvm::Value* res;
            switch (node.op_type)
            {
                case AstRelationalExpr::OpType::GREATER:
                {
                    res =  builder->CreateICmpSGT(node.rela_expr->codegen(*this)->value, node.shift_expr->codegen(*this)->value, "gt");
                    break;
                }
                case AstRelationalExpr::OpType::LESS:
                {
                    res =  builder->CreateICmpSLT(node.rela_expr->codegen(*this)->value, node.shift_expr->codegen(*this)->value, "lt");
                    break;
                }
                case AstRelationalExpr::OpType::GE:
                {
                    res =  builder->CreateICmpSGE(node.rela_expr->codegen(*this)->value, node.shift_expr->codegen(*this)->value, "gte");
                    break;
                }
                case AstRelationalExpr::OpType::LE:
                {
                    res =  builder->CreateICmpSLE(node.rela_expr->codegen(*this)->value, node.shift_expr->codegen(*this)->value, "lte");
                    break;
                }
            }
            return std::make_shared<Variable>(res, nullptr);
        }
    }
    return nullptr;
}

std::shared_ptr<Variable> Visitor::codegen(const AstEqualityExpr& node)
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
                    llvm::Value* res =  builder->CreateICmpEQ(node.equal_expr->codegen(*this)->value, node.rela_expr->codegen(*this)->value, "eq");
                    return std::make_shared<Variable>(res, nullptr);
                }            
                case AstEqualityExpr::OpType::NE:
                {
                    llvm::Value* res =  builder->CreateICmpNE(node.equal_expr->codegen(*this)->value, node.rela_expr->codegen(*this)->value, "ne");
                    return std::make_shared<Variable>(res, nullptr);
                }
            }
        }
    }
    return nullptr;
}

std::shared_ptr<Variable> Visitor::codegen(const AstAndExpr& node)
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

std::shared_ptr<Variable> Visitor::codegen(const AstExclusiveExpr& node)
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

std::shared_ptr<Variable> Visitor::codegen(const AstInclusiveExpr& node)
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

std::shared_ptr<Variable> Visitor::codegen(const AstLogicalAndExpr& node)
{
    switch (node.expr_type)
    {
        case AstLogicalAndExpr::ExprType::INCLUSIVE:
        {
            return node.inclusive_expr->codegen(*this);
        }
        case AstLogicalAndExpr::ExprType::OP:
        {
            llvm::Value* res = builder->CreateLogicalAnd(node.and_expr->codegen(*this)->value, node.inclusive_expr->codegen(*this)->value, "logical_and");
            return std::make_shared<Variable>(res, nullptr);
        }
    }
    return nullptr;
}

std::shared_ptr<Variable> Visitor::codegen(const AstLogicalOrExpr& node)
{
    switch (node.expr_type)
    {
        case AstLogicalOrExpr::ExprType::LOG_AND:
        {
            return node.and_expr->codegen(*this);
        }
        case AstLogicalOrExpr::ExprType::OP:
        {
            llvm::Value* res = builder->CreateLogicalOr(node.or_expr->codegen(*this)->value, node.and_expr->codegen(*this)->value, "logical_or");
            return std::make_shared<Variable>(res, nullptr);
        }
    }
    return nullptr;
}

std::shared_ptr<Variable> Visitor::codegen(const AstConditionalExpr& node)
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

std::shared_ptr<Variable> Visitor::codegen(const AstAssignmentExpr& node)
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
            auto val = this->builder->CreateStore(node.assign_expr->codegen(*this)->value, node.unary_expr->codegen(*this)->addr, false);
            // return std::make_shared<Variable>(node.assign_expr->codegen(*this)->value, nullptr);
            return std::make_shared<Variable>(val, nullptr);
        }
    }
    return nullptr;
}

std::shared_ptr<Variable> Visitor::codegen(const AstExpr& node)
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

std::shared_ptr<Variable> Visitor::codegen(const AstDecl& node)
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
                    auto initializer_value = initializer->codegen(*this)->value;
                      
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
                    llvm::GlobalValue::ExternalLinkage,
                    initializer_v,
                    var_name
                );
                
                present_env->locals.insert({var_name, var});
                
                return std::make_shared<Variable>(nullptr, var);
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
                    auto initializer_value = initializer->codegen(*this)->value;
                    llvm::Value* store_inst = this->builder->CreateStore(initializer_value, var, false);
                }

                present_env->locals.insert({var_name, var});
                return std::make_shared<Variable>(nullptr, var);
            }
        }
    }
    return nullptr;
}

std::shared_ptr<Variable> Visitor::codegen(const AstInitializer& node)
{
    return node.assignment_expr->codegen(*this);
}

std::shared_ptr<Variable> Visitor::codegen(const AstStmt& node)
{
    
    switch (node.stmt_type)
    {
    case AstStmt::StmtType::EXPR :
        return node.expr_stmt->codegen(*this);
        break;
    
    case AstStmt::StmtType::JUMP:
        return node.jump_stmt->codegen(*this);
        break;

    case AstStmt::StmtType::SELECT:
        return node.selection_stmt->codegen(*this);
        break;

    case AstStmt::StmtType::COMPOUND:
        return node.compound_stmt->codegen(*this);
        break;

    default:
        return nullptr;
        break;
    }
}

std::shared_ptr<Variable> Visitor::codegen(const AstCompoundStmt& node)
{
    auto env = new LocalEnv();
    this->envs.push_back(env);

    auto ret = std::shared_ptr<Variable>(nullptr);
    if (node.decl_list)
    {
        ret = node.decl_list->codegen(*this);
    }

    if (node.stmt_list)
    {
        ret = node.stmt_list->codegen(*this);
    }

    this->envs.pop_back();
    return ret;
}

std::shared_ptr<Variable> Visitor::codegen(const AstSelectionStmt& node)
{
    auto cond = node.expr->codegen(*this)->value;
    if(!cond)
    {
        return nullptr;
    }
    if(node.stmt_type == AstSelectionStmt::StmtType::IF_ELSE)
    {
        llvm::Function* parent_function = builder->GetInsertBlock()->getParent();

        llvm::BasicBlock* true_block = llvm::BasicBlock::Create(*context, "then", parent_function);
        llvm::BasicBlock* false_block = llvm::BasicBlock::Create(*context, "else");
        llvm::BasicBlock* merge_block = llvm::BasicBlock::Create(*context, "ifcont");

        builder->CreateCondBr(cond, true_block, false_block);

        // then block
        builder->SetInsertPoint(true_block);
        auto true_class = node.stmt1->codegen(*this);
        llvm::Value* true_value = nullptr;
        if(true_class)
        {
            true_value = true_class->value;
        }

        builder->CreateBr(merge_block);
        true_block = builder->GetInsertBlock();

        // else block
        parent_function->getBasicBlockList().push_back(false_block);
        builder->SetInsertPoint(false_block);
        
        llvm::Value* false_value = nullptr;
        auto false_class = node.stmt2->codegen(*this);
        if(false_class)
        {
            false_value = false_class->value;
        }
        builder->CreateBr(merge_block);

        false_block = builder->GetInsertBlock();
        parent_function->getBasicBlockList().push_back(merge_block);
        builder->SetInsertPoint(merge_block);
        return false_class;
        // llvm::PHINode* pn = builder->CreatePHI(true_value->getType(), 2, "iftmp");
        // std::cout << true_value << " " << false_value << std::endl;
        // pn->addIncoming(true_value, true_block);
        // pn->addIncoming(false_value, false_block);
        // return std::make_shared<Variable>(pn, nullptr);;
    }
    else if(node.stmt_type == AstSelectionStmt::StmtType::IF)
    {
        llvm::Function* parent_function = builder->GetInsertBlock()->getParent();

        llvm::BasicBlock* true_block = llvm::BasicBlock::Create(*context, "then", parent_function);
        llvm::BasicBlock* merge_block = llvm::BasicBlock::Create(*context, "ifcont");

        builder->CreateCondBr(cond, true_block, merge_block);

        // then block
        builder->SetInsertPoint(true_block);
        
        auto true_class = node.stmt1->codegen(*this);
        llvm::Value* true_value = nullptr;
        if(true_class)
        {
            true_value = true_class->value;
        }

        // merge block
        builder->CreateBr(merge_block);
        true_block = builder->GetInsertBlock();

        parent_function->getBasicBlockList().push_back(merge_block);
        builder->SetInsertPoint(merge_block);
        return true_class;
    }
}

std::shared_ptr<Variable> Visitor::codegen(const AstIterStmt& node)
{
    auto cond = node.expr->codegen(*this)->value;
    if(!cond)
    {
        return nullptr;
    }
    if(node.stmt_type == AstIterStmt::StmtType::WHILE)
    {
        llvm::Function* parent_function = builder->GetInsertBlock()->getParent();

        llvm::BasicBlock* true_block = llvm::BasicBlock::Create(*context, "then", parent_function);
        llvm::BasicBlock* merge_block = llvm::BasicBlock::Create(*context, "ifcont");

        builder->CreateCondBr(cond, true_block, merge_block);

        // then block
        builder->SetInsertPoint(true_block);
        
        auto true_class = node.stmt->codegen(*this);
        llvm::Value* true_value = nullptr;
        if(true_class)
        {
            true_value = true_class->value;
        }

        // merge block
        builder->CreateBr(merge_block);
        true_block = builder->GetInsertBlock();

        parent_function->getBasicBlockList().push_back(merge_block);
        builder->SetInsertPoint(merge_block);
        return true_class;
    }
}

std::shared_ptr<Variable> Visitor::codegen(const AstDeclList& node)
{
    auto ret = std::shared_ptr<Variable>(nullptr);
    for (auto decl : node.decls)
    {
        if (decl)
        {
            ret = decl->codegen(*this);
        }
    }
    return ret;
}

std::shared_ptr<Variable> Visitor::codegen(const AstStmtList& node)
{
    auto ret = std::shared_ptr<Variable>(nullptr);
    for (auto stmt : node.stmts)
    {
        if (stmt)
        {
            ret = stmt->codegen(*this);
        }
    }
    return ret;
}

std::shared_ptr<Variable> Visitor::codegen(const AstTranslationUnit& node)
{
    for (auto i : node.external_decl_list)
    {
        i->codegen(*this);
    }
    return nullptr;
}

std::shared_ptr<Variable> Visitor::codegen(const AstExternDecl& node)
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

std::shared_ptr<Variable> Visitor::codegen(const AstFunctionDef& node)
{
    llvm::Type* result_type = llvm::Type::getInt32Ty(*context);
    auto new_param = new std::map<std::string, unsigned>();

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
            unsigned para_num = 0;
            for (auto param_decl : parameter_list_node->parameter_list)
            {
                auto type_spec = param_decl->decl_specifiers->type_specs[0];
                auto type = type_spec->codegen(*this);
                param_types.push_back(type);
                std::string para_name = "";
                if (param_decl->declarator)
                {
                    para_name = param_decl->declarator->direct_declarator->id_name;
                    new_param->insert({para_name, para_num++});
                }
                else
                {
                    para_num++;
                }
            }
            func_type = llvm::FunctionType::get(result_type, param_types, isVarArg);
            break;
        }
    }
    
    llvm::Function* function = llvm::Function::Create(func_type, llvm::GlobalValue::ExternalLinkage, direct_declarator->id_name, &*this->module);
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context, "entry", function, nullptr);
    llvm::BasicBlock* oldBlock = this->builder->GetInsertBlock();
    this->builder->SetInsertPoint(entry);
    auto old_function = this->present_function;
    auto old_function_params = this->func_params;
    this->present_function = function;
    this->func_params = new_param;

    node.compound_stmt->codegen(*this);
    this->envs.back()->functions.insert({direct_declarator->id_name, function});
    this->envs.back()->function_types.insert({direct_declarator->id_name, func_type});
    this->present_function = old_function;
    delete this->func_params;
    this->func_params = old_function_params;
    this->builder->SetInsertPoint(oldBlock);

    return nullptr;
}

std::shared_ptr<Variable> Visitor::codegen(const AstExprStmt& node)
{
    return node.expr->codegen(*this);
}

std::shared_ptr<Variable> Visitor::codegen(const AstJumpStmt& node)
{
    switch (node.stmt_type)
    {
    case AstJumpStmt::StmtType::RETURN:
    {
        this->builder->CreateRetVoid();
        break;
    }
    case AstJumpStmt::StmtType::RETURN_VALUE:
    {
        llvm::Value* ret = node.expr->codegen(*this)->value;
        this->builder->CreateRet(ret);
        break;
    }
    default:
    {
        break;
    }
    }
    return nullptr;
}
// ------------------------------------------------------------------------------

