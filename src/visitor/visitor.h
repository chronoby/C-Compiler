#ifndef VISITOR_H_
#define VISITOR_H_

#include <map>
#include <stack>
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"
#include "../ast/ast.h"

class Visitor
{
public:
    Visitor();
    void codegenProgram(AstPrimaryExpr* root);
    void configureTarget();

    virtual llvm::Value* codegen(const AstInt& node);
    virtual llvm::Value* codegen(const AstIdentifier& node);
    virtual llvm::Value* codegen(const AstAssignment& node);
    virtual llvm::Value* codegen(const AstExprStmt& node);
    virtual llvm::Value* codegen(AstVariableDeclaration& node);
    virtual llvm::Value* codegen(const AstBlock& node);

    virtual llvm::Value* codegen(const AstPrimaryExpr& node);
    virtual llvm::Value* codegen(const AstProfixExpr& node);
    virtual llvm::Value* codegen(const AstExpr& node);

protected:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<> > builder;
    std::unique_ptr<llvm::Module> module;
    
    llvm::BasicBlock* block;
    std::map<std::string, llvm::Value*> locals;

};

#endif // VISITOR_H_
