#ifndef VISITOR_H_
#define VISITOR_H_

#include <map>
#include <stack>
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"
#include "../ast/ast.h"

class LocalEnv
{
public:
    std::map<std::string, llvm::Value*> locals;
};

class Visitor
{
public:
    Visitor();
    void codegenProgram(AstTranslationUnit* root);
    void configureTarget();

    virtual llvm::Value* codegen(const AstPrimaryExpr& node);
    virtual llvm::Value* codegen(const AstPostfixExpr& node);
    virtual llvm::Value* codegen(const AstUnaryExpr& node);
    virtual llvm::Value* codegen(const AstCastExpr& node);
    virtual llvm::Value* codegen(const AstMultiplicativeExpr& node);
    virtual llvm::Value* codegen(const AstAdditiveExpr& node);
    virtual llvm::Value* codegen(const AstShiftExpr& node);
    virtual llvm::Value* codegen(const AstRelationalExpr& node);
    virtual llvm::Value* codegen(const AstEqualityExpr& node);
    virtual llvm::Value* codegen(const AstAndExpr& node);
    virtual llvm::Value* codegen(const AstExclusiveExpr& node);
    virtual llvm::Value* codegen(const AstInclusiveExpr& node);
    virtual llvm::Value* codegen(const AstLogicalAndExpr& node);
    virtual llvm::Value* codegen(const AstLogicalOrExpr& node);
    virtual llvm::Value* codegen(const AstConditionalExpr& node);
    virtual llvm::Value* codegen(const AstAssignmentExpr& node);
    virtual llvm::Value* codegen(const AstExpr& node);

    virtual llvm::Value* codegen(const AstExternDecl& node);
    virtual llvm::Value* codegen(const AstDecl& node);
    llvm::Type* codegen(const AstTypeSpecifier& node);
    virtual llvm::Value* codegen(const AstInitializer& node);
    virtual llvm::Value* codegen(const AstTranslationUnit& node);
    virtual llvm::Value* codegen(const AstFunctionDef& node);

    virtual llvm::Value* codegen(const AstCompoundStmt& node);
    virtual llvm::Value* codegen(const AstDeclList& node);
    virtual llvm::Value* codegen(const AstStmtList& node);

    virtual llvm::Value* codegen(const AstStmt& node);


protected:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<> > builder;
    std::unique_ptr<llvm::Module> module;
    
    llvm::BasicBlock* block;
    
    // std::map<std::string, llvm::Value*> locals;
    std::vector<LocalEnv*> envs;

};

#endif // VISITOR_H_
