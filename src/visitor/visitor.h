#ifndef VISITOR_H_
#define VISITOR_H_

#include <map>
#include <stack>
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"
#include "../ast/ast.h"

class Variable
{
public:
    llvm::Value* value;
    llvm::Value* addr;

    Variable(llvm::Value* value, llvm::Value* addr): value(value), addr(addr) {}
};

class LocalEnv
{
public:
    std::map<std::string, llvm::Value*> locals;
    std::map<std::string, llvm::Function*> functions;
    std::map<std::string, llvm::FunctionType*> function_types;
};

class Visitor
{
public:
    Visitor();
    void codegenProgram(AstTranslationUnit* root);
    void configureTarget();

    virtual std::shared_ptr<Variable> codegen(const AstPrimaryExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstPostfixExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstUnaryExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstCastExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstMultiplicativeExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstAdditiveExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstShiftExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstRelationalExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstEqualityExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstAndExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstExclusiveExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstInclusiveExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstLogicalAndExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstLogicalOrExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstConditionalExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstAssignmentExpr& node);
    virtual std::shared_ptr<Variable> codegen(const AstExpr& node);

    virtual std::shared_ptr<Variable> codegen(const AstExternDecl& node);
    virtual std::shared_ptr<Variable> codegen(const AstDecl& node);
    llvm::Type* codegen(const AstTypeSpecifier& node);
    virtual std::shared_ptr<Variable> codegen(const AstInitializer& node);
    virtual std::shared_ptr<Variable> codegen(const AstTranslationUnit& node);
    virtual std::shared_ptr<Variable> codegen(const AstFunctionDef& node);

    virtual std::shared_ptr<Variable> codegen(const AstCompoundStmt& node);
    virtual std::shared_ptr<Variable> codegen(const AstDeclList& node);
    virtual std::shared_ptr<Variable> codegen(const AstStmtList& node);

    virtual std::shared_ptr<Variable> codegen(const AstStmt& node);
    virtual std::shared_ptr<Variable> codegen(const AstSelectionStmt& node);
    virtual std::shared_ptr<Variable> codegen(const AstExprStmt& node);
    virtual std::shared_ptr<Variable> codegen(const AstJumpStmt& node);
    

    int getTmpVarId();


protected:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<> > builder;
    std::unique_ptr<llvm::Module> module;
    
    // llvm::BasicBlock* block;
    
    // std::map<std::string, llvm::Value*> locals;
    std::vector<LocalEnv*> envs;

    llvm::Function* present_function;
    std::map<std::string, unsigned>* func_params;

    int tmp_var_id = 0;
};

#endif // VISITOR_H_
