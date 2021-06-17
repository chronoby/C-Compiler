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
    CastOrder =
    {
        { llvm::Type::getInt8Ty(*context), 0 },
        { llvm::Type::getInt16Ty(*context), 1 },
        { llvm::Type::getInt32Ty(*context), 2 },
        { llvm::Type::getFloatTy(*context), 3 },
        { llvm::Type::getDoubleTy(*context), 4 }
    };

    envs.push_back(new LocalEnv());
}

void Visitor::codegenProgram(AstTranslationUnit* root, const char* filename)
{
    root->codegen(*this);

    if (this->error)
    {
        std::cerr << "aborted" << std::endl;
        exit(1);
    }

    std::string llvm_IR;
    llvm::raw_string_ostream OS(llvm_IR);
    OS << *module;
    OS.flush();

    std::ofstream outfile(filename);
    outfile << llvm_IR;
    outfile.close();
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
            std::string str_content = node.value;
            str_content = str_content.substr(1, str_content.length() - 2);

            char buf[256];
            int ptr = 0;

            for (int i = 0; i < str_content.size() && i < 256; i++)
            {
                if (str_content[i] == '\\')
                {
                    if (i < str_content.size() - 1 && str_content[i + 1] == '\\')
                    {
                        buf[ptr++] = '\\';
                        i++;
                    }
                    else if (i < str_content.size() - 1 && str_content[i + 1] != '\\')
                    {
                        switch (str_content[i + 1])
                        {
                        case 'a':
                            buf[ptr++] = '\a';
                            i++;
                            break;

                        case 'b':
                            buf[ptr++] = '\b';
                            i++;
                            break;

                        case 'f':
                            buf[ptr++] = '\f';
                            i++;
                            break;

                        case 'n':
                            buf[ptr++] = '\n';
                            i++;
                            break;

                        case 'r':
                            buf[ptr++] = '\r';
                            i++;
                            break;

                        case 't':
                            buf[ptr++] = '\t';
                            i++;
                            break;

                        case '0':
                            buf[ptr++] = 0;
                            i++;
                            break;

                        case 'v':
                            buf[ptr++] = '\v';
                            i++;
                            break;

                        default:
                            buf[ptr++] = '\\';
                        }
                    }
                }
                else
                {
                    buf[ptr++] = str_content[i];
                }
            }
            buf[ptr] =  0;
            
            str_content = std::string(buf);

            int content_len = str_content.length();
            if (content_len > 255) {
                node.warningMsg("String literal is too long. It is cut to length of 255.");
                str_content = str_content.substr(0, 255);
                content_len = str_content.size();
            }

            llvm::Value* str_mem = this->builder->CreateGlobalStringPtr(str_content, "", 0, &*this->module);
            value = str_mem;
            break;
        }
        default:
        {
            node.errorMsg("invalid datatype");
            this->error = 1;
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
            }
        }

        for (int i = this->envs.size() - 1; i >= 0; i--)
        {
            auto env = this->envs[i];
            auto pair = env->locals.find(node.identifier_name);
            if (pair != env->locals.end())
            {
                addr = pair->second;
                if (this->envs.size() != 1) value = this->builder->CreateLoad(addr);

                if (value->getType()->isArrayTy())
                {
                    return std::make_shared<Variable>(addr, nullptr);
                }

                return std::make_shared<Variable>(value, addr);
            }
            if (i == 0)
            {
                node.errorMsg(std::string("identifier not defined: ") + node.identifier_name); 
                this->error = 1; 
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
        node.errorMsg(std::string("invalid primary expression")); 
        this->error = 1; 
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
        case AstPostfixExpr::ExprType::IDX:
        {
            auto post_value = node.postfix_expr->codegen(*this);
            auto ind_value = node.expr->codegen(*this);
            auto zero = llvm::ConstantInt::get(*context, llvm::APInt(32, 0, true));
            // auto index = llvm::ConstantInt::get(*context, llvm::APInt(32, 3, true));
            // auto addr = llvm::GetElementPtrInst::Create(post_value->addr, { zero, index });

            llvm::Value* addr;

            if (!(post_value->value->getType()->getPointerElementType()->isArrayTy()))
            {
                addr = builder->CreateGEP(post_value->value->getType()->getPointerElementType(), post_value->value, ind_value->value);
            }
            else
            {
                addr = builder->CreateGEP(post_value->value, {zero, ind_value->value});
            }

            auto val = builder->CreateLoad(addr);
            return std::make_shared<Variable>(val, addr);
        }
        case AstPostfixExpr::ExprType::OP:
        {
            auto post_value = node.postfix_expr->codegen(*this);
            llvm::Value* value = post_value->value;
            
            llvm::ConstantInt* one = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 1, true);
            llvm::Value* res = nullptr;
            if (node.op_type == AstPostfixExpr::OpType::INC)
            {
                res = this->builder->CreateAdd(value, one);
            }
            else if (node.op_type == AstPostfixExpr::OpType::DEC)
            {
                res = this->builder->CreateSub(value, one);
            }
            if (post_value->addr == nullptr)
            {
                node.errorMsg("invalid left value");
                this->error=1;
                return nullptr;
            }
            this->builder->CreateStore(res, post_value->addr);
            return std::make_shared<Variable>(value, nullptr);
        }
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
                    node.errorMsg(std::string("function not found: ") + node.identifier_name); 
                    this->error = 1; 
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

            static llvm::Function* scanf_func = nullptr;
            static llvm::Function* printf_func = nullptr;

            if (node.identifier_name == "scanf")
            {
                if (scanf_func == nullptr)
                {
                    std::vector<llvm::Type*> arg_types = { llvm::Type::getInt8PtrTy(*context) };
                    func_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context), arg_types, true);
                    func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, "scanf", &*(this->module));
                    func->setCallingConv(llvm::CallingConv::C);
                    scanf_func = func;
                }
                std::vector<llvm::Value*> scanf_args;

                auto expr_list = node.argument_expr_list;
                if (expr_list)
                {
                    for (auto expr : expr_list->expr_list)
                    {
                        auto arg = expr->codegen(*this);
                        if (arg && arg->value) scanf_args.push_back(arg->value);
                        else 
                        {
                            node.errorMsg("invalid function arg");
                            this->error=1;
                        }
                    }
                }

                llvm::Value* ret = this->builder->CreateCall(scanf_func, scanf_args, "scanf_call");
                return std::make_shared<Variable>(ret, nullptr);
            }
            else if (node.identifier_name == "printf")
            {
                if (printf_func == nullptr)
                {
                    std::vector<llvm::Type*> arg_types = { llvm::Type::getInt8PtrTy(*context) };
                    func_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context), arg_types, true);
                    func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, "printf", &*(this->module));
                    func->setCallingConv(llvm::CallingConv::C);
                    printf_func = func;
                }

                std::vector<llvm::Value*> printf_args;

                auto expr_list = node.argument_expr_list;
                if (expr_list)
                {
                    for (auto expr : expr_list->expr_list)
                    {
                        auto arg = expr->codegen(*this);
                        if (arg&& arg->value) printf_args.push_back(arg->value);
                        else 
                        {
                            node.errorMsg("invalid function arg");
                            this->error=1;
                        }
                    }
                }

                llvm::Value* ret = this->builder->CreateCall(printf_func, printf_args, "printf_call");
                return std::make_shared<Variable>(ret, nullptr);
            }

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
                    node.errorMsg(std::string("function not found: ") + node.identifier_name); 
                    this->error = 1; 
                    return nullptr;
                }
            }
            std::vector<llvm::Value*> args;
            auto expr_list = node.argument_expr_list;
            if (expr_list)
            {
                for (auto expr : expr_list->expr_list)
                {
                    auto arg = expr->codegen(*this);
                    // auto copied_arg = this->builder->CreateAlloca(arg->getType(), nullptr, std::string("tmp_var_") + std::to_string(this->getTmpVarId()));
                    // TODO: add type check
                    if (arg && arg->value) args.push_back(arg->value);
                    else 
                    {
                        node.errorMsg("invalid function arg");
                        this->error=1;
                    }
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
        case AstUnaryExpr::ExprType::OP:
        {
            auto unary_value = node.unary_expr->codegen(*this);
            llvm::Value* value = unary_value->value;
            
            llvm::ConstantInt* one = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context), 1, true);
            llvm::Value* res = nullptr;
            if (node.op_type == AstUnaryExpr::OpType::INC)
            {
                res = this->builder->CreateAdd(value, one);
            }
            else if (node.op_type == AstUnaryExpr::OpType::DEC)
            {
                res = this->builder->CreateSub(value, one);
            }
            if (unary_value->addr == nullptr)
            {
                node.errorMsg("invalid left value");
                this->error=1;
                return nullptr;
            }
            this->builder->CreateStore(res, unary_value->addr);
            return std::make_shared<Variable>(res, nullptr);
        }
        case AstUnaryExpr::ExprType::UNARY_OP:
        {
            switch (node.unary_op->type)
            {
            case AstUnaryOp::OpType::AND:
            {
                std::shared_ptr<Variable> id = node.cast_expr->codegen(*this);
                llvm::Value* ptr_value = id->addr;
                if (ptr_value == nullptr)
                {
                    node.errorMsg(std::string("cannot get the address of this unary expression")); 
                    this->error = 1; 
                    return nullptr; 
                } 
                return std::make_shared<Variable>(ptr_value, nullptr);

                break;
            }
            case AstUnaryOp::OpType::STAR:
            {
                std::shared_ptr<Variable> id = node.cast_expr->codegen(*this);
                llvm::Value* value_ptr = id->value;
                llvm::Value* value = this->builder->CreateLoad(value_ptr);
                return std::make_shared<Variable>(value, value_ptr);
                break;
            }
            case AstUnaryOp::OpType::MINUS:
            {
                std::shared_ptr<Variable> id = node.cast_expr->codegen(*this);
                
                llvm::Value* v = id->value;
                llvm::Type* v_type = v->getType();
                
                llvm::Constant* minus_1 = nullptr;
                if (v_type->isFloatingPointTy()) minus_1 = llvm::ConstantFP::get(v_type, -1.0);
                else minus_1 = llvm::ConstantInt::get(v_type, -1);

                llvm::Value* res = this->builder->CreateMul(v, minus_1);
                return std::make_shared<Variable>(res, nullptr);
            }
            default:
                break;
            }
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
        case AstCastExpr::ExprType::CAST:
        {
            std::shared_ptr<Variable> before_cast = node.cast_expr->codegen(*this);
            llvm::Type* type = node.type_spec->codegen(*this);
            if (before_cast)
            {
                llvm::Instruction::CastOps cast_op = llvm::CastInst::getCastOpcode(before_cast->value, true, type, true);
                bool able_to_cast = llvm::CastInst::castIsValid(cast_op, before_cast->value->getType(), type);
                if (!able_to_cast)
                {
                    node.errorMsg("unable to do implict cast between these types");
                    this->error=1;
                    return nullptr;
                }
                llvm::Value* cast_value = this->builder->CreateCast(cast_op, before_cast->value, type);
                return std::make_shared<Variable>(cast_value, nullptr);
            }
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
            auto lhs = node.multi_expr->codegen(*this)->value;
            auto rhs = node.cast_expr->codegen(*this)->value;
            
            switch (node.op_type)
            {
                case AstMultiplicativeExpr::OpType::MUL:
                {
                    auto cast_res = type_check(lhs, rhs);
                    if(cast_res == CastRes::INT)
                    {
                        res = this->builder->CreateMul(lhs, rhs, "mul");
                    }
                    else if(cast_res == CastRes::FLOAT)
                    {
                        res = this->builder->CreateFMul(lhs, rhs, "fmul");
                    }
                    else if(cast_res == CastRes::WRONG)
                    {
                        node.errorMsg("unable to do implict cast between these types");
                        this->error = 1;
                        return nullptr;
                    }
                    else
                    {
                        return nullptr;
                    }
                    break;
                }
                case AstMultiplicativeExpr::OpType::DIV:
                {
                    auto cast_res = type_check(lhs, rhs);
                    if(cast_res == CastRes::INT)
                    {
                        res = this->builder->CreateSDiv(lhs, rhs, "sdiv");
                    }
                    else if(cast_res == CastRes::FLOAT)
                    {
                        res = this->builder->CreateFDiv(lhs, rhs, "fdiv");
                    }
                    else if(cast_res == CastRes::WRONG)
                    {
                        node.errorMsg("unable to do implict cast between these types");
                        this->error = 1;
                        return nullptr;
                    }
                    else
                    {
                        return nullptr;
                    }
                    break;
                }
                case AstMultiplicativeExpr::OpType::MOD:
                {
                    if(lhs->getType()->isIntegerTy() && rhs->getType()->isIntegerTy())
                    {
                        res = this->builder->CreateSRem(lhs, rhs, "ram");
                    }
                    else
                    {
                        node.errorMsg("mod operands type is wrong");
                        this->error = 1;
                        return nullptr;
                    }
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
            auto lhs = node.add_expr->codegen(*this)->value;
            auto rhs = node.multi_expr->codegen(*this)->value;
            auto cast_res = type_check(lhs, rhs);
            switch (node.op_type)
            {
                case AstAdditiveExpr::OpType::PLUS:
                {
                    if(cast_res == CastRes::INT)
                    {
                        res = this->builder->CreateAdd(lhs, rhs, "add");
                    }
                    else if(cast_res == CastRes::FLOAT)
                    {
                        res = this->builder->CreateFAdd(lhs, rhs, "fadd");
                    }
                    else if(cast_res == CastRes::WRONG)
                    {
                        node.errorMsg("unable to do implict cast between these types");
                        this->error = 1;
                        return nullptr;
                    }
                    else
                    {
                        // true?
                        // unknown type
                        return nullptr;
                    }
                    break;
                }
                case AstAdditiveExpr::OpType::MINUS:
                {
                    if(cast_res == CastRes::INT)
                    {
                        res = this->builder->CreateSub(lhs, rhs, "sub");
                    }
                    else if(cast_res == CastRes::FLOAT)
                    {
                        res = this->builder->CreateFSub(lhs, rhs, "fsub");
                    }
                    else if(cast_res == CastRes::WRONG)
                    {
                        node.errorMsg("unable to do implict cast between these types");
                        this->error = 1;
                        return nullptr;
                    }
                    else
                    {
                        return nullptr;
                    }
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
            auto lhs = node.rela_expr->codegen(*this)->value;
            auto rhs = node.shift_expr->codegen(*this)->value;
            auto cast_res = type_check(lhs, rhs);
            
            if(cast_res == CastRes::INT)
            {
                switch (node.op_type)
                {
                    case AstRelationalExpr::OpType::GREATER:
                    {
                        res =  builder->CreateICmpSGT(lhs, rhs, "gt");
                        break;
                    }
                    case AstRelationalExpr::OpType::LESS:
                    {
                        res =  builder->CreateICmpSLT(lhs, rhs, "lt");
                        break;
                    }
                    case AstRelationalExpr::OpType::GE:
                    {
                        res =  builder->CreateICmpSGE(lhs, rhs, "gte");
                        break;
                    }
                    case AstRelationalExpr::OpType::LE:
                    {
                        res =  builder->CreateICmpSLE(lhs, rhs, "lte");
                        break;
                    }
                }
            }
            else if (cast_res == CastRes::FLOAT)
            {
                switch (node.op_type)
                {
                    case AstRelationalExpr::OpType::GREATER:
                    {
                        res =  builder->CreateFCmpOGT(lhs, rhs, "fgt");
                        break;
                    }
                    case AstRelationalExpr::OpType::LESS:
                    {
                        res =  builder->CreateFCmpOLT(lhs, rhs, "flt");
                        break;
                    }
                    case AstRelationalExpr::OpType::GE:
                    {
                        res =  builder->CreateFCmpOGE(lhs, rhs, "fgte");
                        break;
                    }
                    case AstRelationalExpr::OpType::LE:
                    {
                        res =  builder->CreateFCmpOLE(lhs, rhs, "flte");
                        break;
                    }
                }
            }
            else if(cast_res == CastRes::WRONG)
            {
                node.errorMsg("unable to do implict cast between these types");
                this->error = 1;
                return nullptr;
            }
            else
            {
                return nullptr;
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
            llvm::Value* res;
            auto lhs = node.equal_expr->codegen(*this)->value;
            auto rhs = node.rela_expr->codegen(*this)->value;
            auto cast_res = type_check(lhs, rhs);
            if(cast_res == CastRes::INT)
            {
                switch (node.op_type)
                {
                    case AstEqualityExpr::OpType::EQ:
                    {
                        llvm::Value* res =  builder->CreateICmpEQ(lhs, rhs, "eq");
                        return std::make_shared<Variable>(res, nullptr);
                    }            
                    case AstEqualityExpr::OpType::NE:
                    {
                        llvm::Value* res =  builder->CreateICmpNE(lhs, rhs, "ne");
                        return std::make_shared<Variable>(res, nullptr);
                    }
                }
            }
            else if (cast_res == CastRes::FLOAT)
            {
                switch (node.op_type)
                {
                    case AstEqualityExpr::OpType::EQ:
                    {
                        llvm::Value* res =  builder->CreateFCmpOEQ(lhs, rhs, "feq");
                        return std::make_shared<Variable>(res, nullptr);
                    }            
                    case AstEqualityExpr::OpType::NE:
                    {
                        llvm::Value* res =  builder->CreateFCmpONE(lhs, rhs, "fne");
                        return std::make_shared<Variable>(res, nullptr);
                    }
                }
            }
            else if(cast_res == CastRes::WRONG)
            {
                node.errorMsg("unable to do implict cast between these types");
                this->error = 1;
                return nullptr;
            }
            else
            {
                return nullptr;
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
            std::shared_ptr<Variable> left = node.unary_expr->codegen(*this);
            std::shared_ptr<Variable> right = node.assign_expr->codegen(*this);

            if (left == nullptr || left->addr == nullptr)
            {
                node.errorMsg("assignment failed: invalid left value");
                this->error = 1;
                return nullptr;
            }
            if (right == nullptr || right->value == nullptr)
            {
                node.errorMsg("assignment failed: invalid right value");
                this->error = 1;
                return nullptr;
            }
            // need update
            // return new llvm::StoreInst(node.assign_expr->codegen(*this), node.unary_expr->codegen(*this), false, block);
            if (left->value->getType() != right->value->getType())
            {
                llvm::Instruction::CastOps cast_op = llvm::CastInst::getCastOpcode(right->value, true, left->value->getType(), true);
                bool able_to_cast = llvm::CastInst::castIsValid(cast_op, right->value->getType(), left->value->getType());
                
                if (!able_to_cast)
                {
                    node.errorMsg("unable to do implict cast between these types");
                    this->error=1;
                    return nullptr;
                }
                llvm::Value* cast_value = this->builder->CreateCast(cast_op, right->value, left->value->getType());
                auto val = this->builder->CreateStore(cast_value, left->addr, false);
                return std::make_shared<Variable>(val, nullptr);
            }
            else
            {
                auto val = this->builder->CreateStore(right->value, left->addr, false);
                // return std::make_shared<Variable>(node.assign_expr->codegen(*this)->value, nullptr);
                return std::make_shared<Variable>(val, nullptr);
            }
            
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
        case (AstTypeSpecifier::Type::VOID):
        {
            return llvm::Type::getVoidTy(*context); 
        }
        default:
        {
            node.errorMsg("invalid data type");
            this->error = 1;
            return nullptr;
        }
    }
}

llvm::Type* Visitor::getPointerType(const AstTypeSpecifier& node)
{
    switch (node.type)
    {
        case (AstTypeSpecifier::Type::INT):
        {
            return llvm::Type::getInt32PtrTy(*context);
        }
        case (AstTypeSpecifier::Type::SHORT):
        {
            return llvm::Type::getInt16PtrTy(*context);
        }
        case (AstTypeSpecifier::Type::LONG):
        {
            return llvm::Type::getInt32PtrTy(*context);
        }
        case (AstTypeSpecifier::Type::DOUBLE):
        {
            return llvm::Type::getDoublePtrTy(*context);
        }
        case (AstTypeSpecifier::Type::CHAR):
        {
            return llvm::Type::getInt8PtrTy(*context);            
        }
        case (AstTypeSpecifier::Type::FLOAT):
        {
            return llvm::Type::getFloatPtrTy(*context);
        }
        default:
        {
            node.errorMsg("invalid data type");
            this->error = 1;
            return nullptr;
        }
    }
}

std::shared_ptr<Variable> Visitor::codegen(const AstDecl& node)
{
    if (!node.decl_specifiers || node.decl_specifiers->type_specs.size() != 1)
    {
        node.errorMsg("invalid declaration specifiers");
        this->error = 1;
        return nullptr;
    }
    auto type_spec = node.decl_specifiers->type_specs[0];
    if (!node.init_declarator_list)
    {
        node.errorMsg("invalid init declarators");
        this->error = 1;
        return nullptr;
    }
    auto init_declarators = node.init_declarator_list->init_declarators;

    for (auto init_declarator : init_declarators)
    {
        auto declarator = init_declarator->declarator;
        auto initializer = init_declarator->initializer;

        std::string var_name = declarator->direct_declarator->id_name;
        llvm::Type* var_type = nullptr;

        var_type = type_spec->codegen(*this);

        if (declarator->declarator_type == AstDeclarator::DeclaratorType::POINTER)
        {
            auto ptr = declarator->pointer;
            while (ptr != nullptr)
            {
                var_type = var_type->getPointerTo();
                ptr = ptr->next;
            }
        }

        if(declarator->direct_declarator->declarator_type == AstDirectDeclarator::DeclaratorType::BR)
        {
            // get array size
            int num = 5;
            auto num_value = declarator->direct_declarator->prime_expr->codegen(*this)->value;
            if (llvm::ConstantInt* CI = llvm::dyn_cast<llvm::ConstantInt>(num_value)) {
                if (CI->getBitWidth() <= 32) {
                    num = CI->getSExtValue();
                }
            }
            // get array type
            var_type = llvm::ArrayType::get(var_type, num);
        }
        
        llvm::Constant* initializer_v = llvm::ConstantAggregateZero::get(var_type);
        if (envs.size() == 1)
        {
            if (initializer != nullptr)
            {
                if(declarator->direct_declarator->declarator_type == AstDirectDeclarator::DeclaratorType::ID)
                {
                    auto initializer_value = initializer->codegen(*this);

                    if (initializer_value!= nullptr && initializer_value->value != nullptr && llvm::isa<llvm::Constant>(initializer_value->value))
                    {
                        initializer_v = llvm::cast<llvm::Constant>(initializer_value->value);
                    }
                    else
                    {
                        node.warningMsg(std::string("global variable") + var_name + "has invalid initializer. It will be initialized with zero");
                    }
                }
                
                else if(declarator->direct_declarator->declarator_type == AstDirectDeclarator::DeclaratorType::BR)
                {
                    std::vector<llvm::Constant*> values;
                    for(auto init : initializer->initializer_list->initializer_list)
                    {
                        auto value = init->codegen(*this)->value;
                        values.push_back(llvm::dyn_cast<llvm::Constant>(value));
                    }
                    initializer_v = llvm::ConstantArray::get(llvm::dyn_cast<llvm::ArrayType>(var_type), values);
                }

            }
            LocalEnv* present_env = envs[0];
            if (present_env->locals.find(var_name) != present_env->locals.end())
            {
                node.errorMsg(std::string("variable redeclaration") + var_name);
                this->error = 1;
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
                node.errorMsg(std::string("variable redeclaration") + var_name);
                this->error = 1;
                return nullptr;
            }

            llvm::AllocaInst* var = this->builder->CreateAlloca(var_type, nullptr, var_name);

            if (initializer)
            {
                if(declarator->direct_declarator->declarator_type == AstDirectDeclarator::DeclaratorType::ID)
                {
                    llvm::Value* initializer_value = initializer->codegen(*this)->value;

                    if (initializer_value->getType() != var->getType()->getPointerElementType())
                    {
                        llvm::Instruction::CastOps cast_op = llvm::CastInst::getCastOpcode(initializer_value, true, var->getType()->getPointerElementType(), true);
                        bool able_to_cast = llvm::CastInst::castIsValid(cast_op, initializer_value->getType(), var->getType()->getPointerElementType());
                        if (!able_to_cast)
                        {
                            node.errorMsg("unable to do implict cast between these types");
                            this->error=1;
                            return nullptr;
                        }
                        initializer_value = this->builder->CreateCast(cast_op, initializer_value, var->getType()->getPointerElementType());
                    }  
                    
                    llvm::Value* store_inst = this->builder->CreateStore(initializer_value, var, false);
                }
                else if(declarator->direct_declarator->declarator_type == AstDirectDeclarator::DeclaratorType::BR)
                {
                    std::vector<llvm::Constant*> values;
                    llvm::Type* elem_type = var_type->getArrayElementType();
                    
                    for(auto init : initializer->initializer_list->initializer_list)
                    {
                        auto value = init->codegen(*this)->value;
                        if (value->getType() != elem_type)
                        {
                            llvm::Instruction::CastOps cast_op = llvm::CastInst::getCastOpcode(value, true, elem_type, true);
                            bool able_to_cast = llvm::CastInst::castIsValid(cast_op, value->getType(), elem_type);
                            if (!able_to_cast)
                            {
                                node.errorMsg("unable to do implict cast between these types");
                                this->error=1;
                                return nullptr;
                            }
                            value = this->builder->CreateCast(cast_op, value, elem_type);
                        }
                        values.push_back(llvm::dyn_cast<llvm::Constant>(value));
                    }
                    auto initializer_value = llvm::ConstantArray::get(llvm::dyn_cast<llvm::ArrayType>(var_type), values);
                    llvm::Value* store_inst = this->builder->CreateStore(initializer_value, var, false);
                }
                
            }

            present_env->locals.insert({var_name, var});
            return std::make_shared<Variable>(nullptr, var);
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
    if(node.stmt_type == AstIterStmt::StmtType::WHILE)
    {
        llvm::Function* parent_function = builder->GetInsertBlock()->getParent();
        llvm::BasicBlock *pre_block = builder->GetInsertBlock();
        llvm::BasicBlock* loop_block = llvm::BasicBlock::Create(*context, "loop", parent_function);
        llvm::BasicBlock* true_block = llvm::BasicBlock::Create(*context, "loopin", parent_function);
        llvm::BasicBlock* cont_block = llvm::BasicBlock::Create(*context, "loopcont");
        tmp_loop_block = loop_block;
        tmp_cont_block = cont_block;

        builder->CreateBr(loop_block);
        builder->SetInsertPoint(loop_block);
        
        auto cond = node.expr->codegen(*this)->value;
        if(!cond)
        {
            return nullptr;
        }
        builder->CreateCondBr(cond, true_block, cont_block);

        // then block
        builder->SetInsertPoint(true_block);
        
        auto true_class = node.stmt->codegen(*this);
        llvm::Value* true_value = nullptr;
        if(true_class)
        {
            true_value = true_class->value;
        }
        builder->CreateBr(loop_block);

        // cont block
        parent_function->getBasicBlockList().push_back(cont_block);
        builder->SetInsertPoint(cont_block);
        tmp_loop_block = nullptr;
        tmp_cont_block = nullptr;
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
    auto direct_declarator = node.declarator->direct_declarator;
    if (envs.back()->function_defined.find(direct_declarator->id_name) != envs.back()->function_defined.end())
    {
        node.errorMsg(std::string("function redefined: ") + direct_declarator->id_name);
        this->error=1;
        return nullptr;
    }
    
    llvm::Type* result_type = llvm::Type::getInt32Ty(*context);
    auto new_param = new std::map<std::string, unsigned>();

    auto decl_specs = node.decl_specifiers;
    if (decl_specs && decl_specs->type_specs.size() > 0)
    {  
        auto type_specs = decl_specs->type_specs[0];
        result_type = type_specs->codegen(*this);
    }
    
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
                if (param_decl->declarator->declarator_type == AstDeclarator::DeclaratorType::POINTER)
                {
                    auto ptr = param_decl->declarator->pointer;
                    while (ptr != nullptr)
                    {
                        type = type->getPointerTo();
                        ptr = ptr->next;
                    }
                }
                
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

    if (this->envs.back()->functions.find(direct_declarator->id_name) == this->envs.back()->functions.end())
    {
        this->envs.back()->functions.insert({direct_declarator->id_name, function});
        this->envs.back()->function_types.insert({direct_declarator->id_name, func_type});
    }
    
    this->envs.back()->function_defined.insert({direct_declarator->id_name, true});
    
    node.compound_stmt->codegen(*this);
    if (result_type->isVoidTy()) this->builder->CreateRetVoid();
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
        llvm::BasicBlock* block = llvm::BasicBlock::Create(*context);
        builder->SetInsertPoint(block);
        break;
    }
    case AstJumpStmt::StmtType::RETURN_VALUE:
    {
        llvm::Value* ret = node.expr->codegen(*this)->value;
        this->builder->CreateRet(ret);
        llvm::BasicBlock* block = llvm::BasicBlock::Create(*context);
        builder->SetInsertPoint(block);
        break;
    }
    case AstJumpStmt::StmtType::CONTINUE:
    {
        builder->CreateBr(tmp_loop_block);
        llvm::BasicBlock* block = llvm::BasicBlock::Create(*context);
        builder->SetInsertPoint(block);
        break;
    }
    case AstJumpStmt::StmtType::BREAK:
    {
        builder->CreateBr(tmp_cont_block);
        llvm::BasicBlock* block = llvm::BasicBlock::Create(*context);
        builder->SetInsertPoint(block);

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

Visitor::CastRes Visitor::type_check(llvm::Value*& lhs, llvm::Value*& rhs)
{
    auto l_type = lhs->getType();
    auto r_type = rhs->getType();
    if(l_type == r_type)
    {
        if(l_type->isIntOrPtrTy())
        {
            return CastRes::INT;
        }
        else if(l_type->isFloatingPointTy())
        {
            return CastRes::FLOAT;
        }
        else
        {
            return CastRes::OTHER;
        }
    }
    else
    {
        // std::cout << l_type << " " << r_type << std::endl;
        // std::cout << llvm::Type::getInt32Ty(*context) << " " << llvm::Type::getInt16Ty(*context) << std::endl;
        // std::cout << CastOrder[l_type] << " " << CastOrder[r_type] << std::endl;
        
        if(CastOrder[l_type] > CastOrder[r_type])
        {
            llvm::Instruction::CastOps cast_op = llvm::CastInst::getCastOpcode(rhs, true, l_type, true);
            bool able_to_cast = llvm::CastInst::castIsValid(cast_op, r_type, l_type);
            if (!able_to_cast)
            {
                return CastRes::WRONG;
            }
            rhs = this->builder->CreateCast(cast_op, rhs, l_type);
            if(l_type->isIntOrPtrTy())
            {
                return CastRes::INT;
            }
            else if(l_type->isFloatingPointTy())
            {
                return CastRes::FLOAT;
            }

        }
        else
        {
            llvm::Instruction::CastOps cast_op = llvm::CastInst::getCastOpcode(lhs, true, r_type, true);
            bool able_to_cast = llvm::CastInst::castIsValid(cast_op, l_type, r_type);
            if (!able_to_cast)
            {
                return CastRes::WRONG;
            }
            lhs = this->builder->CreateCast(cast_op, lhs, r_type);
            if(r_type->isIntOrPtrTy())
            {
                return CastRes::INT;
            }
            else if(r_type->isFloatingPointTy())
            {
                return CastRes::FLOAT;
            }
        }
    }
}
