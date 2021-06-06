#ifndef AST_H_
#define AST_H_

#include <vector>
#include <memory>
#include <string>
#include <llvm/IR/Value.h>

class Visitor;

class AstNode
{
public:
    virtual llvm::Value* codegen(Visitor& visitor) = 0;
};

class AstExpression : public AstNode { };
class AstStatement : public AstNode { };

class AstInt : public AstExpression
{
public:
    AstInt(int v) : val(v) { }
    virtual llvm::Value* codegen(Visitor& visitor) override;
    int getInt() const { return val; }
    int val;
};

class AstIdentifier : public AstExpression
{
public:
    AstIdentifier(std::string n) : name(n) { }
    virtual llvm::Value* codegen(Visitor& visitor) override;
    std::string getName() const { return name; }
// private:
    std::string name;
};

class AstAssignment : public AstExpression
{
public:
    AstAssignment(AstIdentifier* id, AstExpression* exp) : lhs(id), rhs(exp) { }
    virtual llvm::Value* codegen(Visitor& visitor) override;
    std::string getName() const { return lhs->getName(); }
    AstExpression* getExpr() const { return rhs; }
private:
    AstIdentifier* lhs;
    AstExpression* rhs;
    int op;
};

class AstExprStmt : public AstStatement
{
public:
    AstExprStmt(AstExpression* exp) : expr(exp) { }
    virtual llvm::Value* codegen(Visitor& visitor) override;
    AstExpression* getExpr() const { return expr; }
private:
    AstExpression* expr;
};

class AstVariableDeclaration : public AstStatement
{
public:
    AstVariableDeclaration(AstIdentifier* tid) : id(tid) { }
    virtual llvm::Value* codegen(Visitor& visitor) override;
    const AstIdentifier* getType() const { return type; }
    AstIdentifier* getId() const { return id; }
// private:
    const AstIdentifier* type;
    AstIdentifier* id;
};

class AstBlock : public AstExpression
{
public:
    virtual llvm::Value* codegen(Visitor& visitor) override;
    std::vector<AstStatement*> getStmtList() const { return stmt_list; }
    void pushStmt(AstStatement* stmt) { stmt_list.push_back(stmt); }
//private:
    std::vector<AstStatement*> stmt_list;
};

#endif // AST_H_
