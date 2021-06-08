#ifndef AST_H_
#define AST_H_

#include <vector>
#include <memory>
#include <string>
#include <llvm/IR/Value.h>

class Visitor;
class AstExpr;

class AstNode
{
public:
    virtual llvm::Value* codegen(Visitor& visitor) = 0;
};

class AstExpression : public AstNode { };
class AstStatement : public AstNode { };

// --------------------------- expression -------------------------

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
    AstPostfixExpr(AstPostfixExpr* expr, std::string ind): postfix_expr(expr), identifier_name(expr->identifier_name), 
        index(ind), expr_type(ExprType::IDX) { }
    AstPostfixExpr(AstPostfixExpr* expr, OpType op): postfix_expr(expr), op_type(op), expr_type(ExprType::OP) { }

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

    AstUnaryExpr(AstPostfixExpr* expr): postfix_expr(expr), expr_type(ExprType::POSTFIX) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;

// private:
    AstPostfixExpr* postfix_expr;
    AstUnaryExpr* unary_expr;
    ExprType expr_type;
    OpType op_type;
};

class AstCastExpr: public AstExpression
{
public:
    enum class ExprType {UNARY, CAST};

    AstCastExpr(AstUnaryExpr* expr): unary_expr(expr), expr_type(ExprType::UNARY) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstUnaryExpr* unary_expr;
    AstCastExpr* cast_expr;
};

class AstMultiplicativeExpr: public AstExpression
{
public:
    enum class ExprType {CAST, OP};
    enum class OpType {MUL, DIV, MOD};

    AstMultiplicativeExpr(AstCastExpr* expr): cast_expr(expr), expr_type(ExprType::CAST) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstCastExpr* cast_expr;
};

class AstAdditiveExpr: public AstExpression
{
public:
    enum class ExprType {MULTI, OP};
    enum class OpType {PLUS, MINUS};

    AstAdditiveExpr(AstMultiplicativeExpr* expr): multi_expr(expr), expr_type(ExprType::MULTI) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstMultiplicativeExpr* multi_expr;
};

class AstShiftExpr: public AstExpression
{
public:
    enum class ExprType {ADD, OP};
    enum class OpType {LEFT, RIGHT};

    AstShiftExpr(AstAdditiveExpr* expr): add_expr(expr), expr_type(ExprType::ADD) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstAdditiveExpr* add_expr;
};

class AstRelationalExpr: public AstExpression
{
public:
    enum class ExprType {SHIFT, OP};
    enum class OpType {LESS, GREATER, LE, GE};

    AstRelationalExpr(AstShiftExpr* expr): shift_expr(expr), expr_type(ExprType::SHIFT) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstShiftExpr* shift_expr;
};

class AstEqualityExpr: public AstExpression
{
public:
    enum class ExprType {RELATIONAL, OP};
    enum class OpType {EQ, NE};

    AstEqualityExpr(AstRelationalExpr* expr): rela_expr(expr), expr_type(ExprType::RELATIONAL) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstRelationalExpr* rela_expr;
};

class AstAndExpr: public AstExpression
{
public:
    enum class ExprType {EQUALITY, OP};

    AstAndExpr(AstEqualityExpr* expr): equal_expr(expr), expr_type(ExprType::EQUALITY) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstEqualityExpr* equal_expr;
};

class AstExclusiveExpr: public AstExpression
{
public:
    enum class ExprType {AND, OP};

    AstExclusiveExpr(AstAndExpr* expr): and_expr(expr), expr_type(ExprType::AND) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstAndExpr* and_expr;
};

class AstInclusiveExpr: public AstExpression
{
public:
    enum class ExprType {EXCLUSIVE, OP};
    AstInclusiveExpr(AstExclusiveExpr* expr): exclusive_expr(expr), expr_type(ExprType::EXCLUSIVE) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstExclusiveExpr* exclusive_expr;
};

class AstLogicalAndExpr: public AstExpression
{
public:
    enum class ExprType {INCLUSIVE, OP};
    AstLogicalAndExpr(AstInclusiveExpr* expr): inclusive_expr(expr), expr_type(ExprType::INCLUSIVE) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstInclusiveExpr* inclusive_expr;
};

class AstLogicalOrExpr: public AstExpression
{
public:
    enum class ExprType {LOG_AND, OP};

    AstLogicalOrExpr(AstLogicalAndExpr* expr): and_expr(expr), expr_type(ExprType::LOG_AND) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstLogicalAndExpr* and_expr;
};

class AstConditionalExpr: public AstExpression
{
public:
    enum class ExprType {LOG_OR, OP};

    AstConditionalExpr(AstLogicalOrExpr* expr): or_expr(expr), expr_type(ExprType::LOG_OR) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstLogicalOrExpr* or_expr;
};

class AstAssignmentExpr: public AstExpression
{
public:
    enum class ExprType {CONDITIONAL, OP};

    AstAssignmentExpr(AstConditionalExpr* expr): cond_expr(expr), expr_type(ExprType::CONDITIONAL) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstConditionalExpr* cond_expr;
};

class AstExpr: public AstExpression
{
public:
    enum class ExprType {ASSIGN, OP};

    AstExpr(AstAssignmentExpr* expr): assign_expr(expr), expr_type(ExprType::ASSIGN) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstAssignmentExpr* assign_expr;
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
