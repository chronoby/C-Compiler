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

// --------------------------- expression -------------------------

class AstExpr : public AstExpression
{
public:
    virtual llvm::Value* codegen(Visitor& visitor) override;
};

class AstArgumentExprList : public AstExpression
{

};

// Type for primary_expr
class AstPrimaryExpr : public AstExpression
{
public:
    // which grammar rule is used to derive this node
    enum class ExprType {ID, CONSTANT, PR_EXPR};
    
    // type of data if it is a constant
    enum class DataType {INTEGER, HEXI, OCTAL, FLOAT, CHAR, STRING};
    
    AstPrimaryExpr(AstPrimaryExpr::DataType dtype, std::string val): value(val), data_type(dtype), expr_type(AstPrimaryExpr::ExprType::CONSTANT) {}
    AstPrimaryExpr(std::string name): identifier_name(name), expr_type(AstPrimaryExpr::ExprType::ID) {}
    AstPrimaryExpr(AstExpr* expr): expr(expr), value(""), expr_type(AstPrimaryExpr::ExprType::PR_EXPR) {}

    virtual llvm::Value* codegen(Visitor& visitor) override;
// private:
    // which grammar rule is used to derive this node
    ExprType expr_type;

    // '('expression')'
    AstExpr* expr;

    // INTEGER | HEXI | OCTAL | ...
    DataType data_type;
    std::string value;

    // INDENTIFIER
    std::string identifier_name;
};

// NOTE: TO BE FINISHED
class AstPostfixExpr : public AstExpression
{
public:
    // which grammar rule is used to derive this node
    enum class ExprType {PRIMARY, IDX, FUNC, FUNC_PARAM, MEMBER, PTR_MEMBER, OP};
    
    // in term of a++ or a--
    enum class OpType {INC, DEC};
    
    // TO BE FINISED
    AstPostfixExpr(AstPrimaryExpr* primary_expr): primary_expr(primary_expr), expr_type(ExprType::PRIMARY) {};
    AstPostfixExpr(AstPostfixExpr* pf_expr, std::string ind): postfix_expr(pf_expr), identifier_name(pf_expr->identifier_name), 
        index(ind), expr_type(ExprType::IDX) { }
    AstPostfixExpr(AstPostfixExpr* pf_expr, OpType op): postfix_expr(pf_expr), op_type(op), expr_type(ExprType::OP) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
// private:
    // which grammar rule is used to derive this node
    ExprType expr_type;

    // TO BE FINISHED
    AstPrimaryExpr* primary_expr;

    AstPostfixExpr* postfix_expr;
    AstArgumentExprList* argument_expr_list;
    AstExpr* expr;
    OpType op_type;

    std::string identifier_name;
    std::string index;
};

class AstUnaryExpr : public AstExpression
{
public:
    enum class ExprType {POSTFIX, OP, CAST, SIZEOF_TYPE, SIZEOF_EXPR};
    enum class OpType {INC, DEC};

    AstUnaryExpr(AstPostfixExpr* pf_expr): postfix_expr(pf_expr), expr_type(ExprType::POSTFIX) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;

// private:
    AstPostfixExpr* postfix_expr;
    AstUnaryExpr* unary_expr;
    ExprType expr_type;
    OpType op_type;
};

// ----------------------------------------------------------------

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
