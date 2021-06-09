#ifndef AST_H_
#define AST_H_

#include <vector>
#include <memory>
#include <string>
#include <map>
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
    enum class ExprType {CONDITIONAL, ASSIGN};

    AstAssignmentExpr(AstConditionalExpr* expr): cond_expr(expr), expr_type(ExprType::CONDITIONAL) { }
    AstAssignmentExpr(AstUnaryExpr* expr, AstAssignmentExpr* a_expr): unary_expr(expr), assign_expr(a_expr),
        expr_type(ExprType::ASSIGN) { }

    virtual llvm::Value* codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstConditionalExpr* cond_expr;
    AstUnaryExpr* unary_expr;
    AstAssignmentExpr* assign_expr;
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

// ---------------------- DECLRATION ---------------------------

class AstTypeSpecifier
{
public:
    enum class Type {
        VOID,
        CHAR,
        SHORT,
        INT,
        LONG,
        FLOAT,
        DOUBLE,
        SIGNED,
        UNSIGNED,
        STRUCT,
        UNION,
        ENUM,
        TYPE_NAME
    };
    Type type;

    std::map<std::string, AstTypeSpecifier::Type> type_map = {
        {"void", AstTypeSpecifier::Type::VOID},
        {"char", AstTypeSpecifier::Type::CHAR},
        {"short", AstTypeSpecifier::Type::SHORT},
        {"int", AstTypeSpecifier::Type::INT},
        {"long", AstTypeSpecifier::Type::LONG},
        {"float", AstTypeSpecifier::Type::FLOAT},
        {"double", AstTypeSpecifier::Type::DOUBLE},
        {"signed", AstTypeSpecifier::Type::SIGNED},
        {"unsigned", AstTypeSpecifier::Type::UNSIGNED}
    };

    AstTypeSpecifier(std::string spec)
    {
        auto find_pair = type_map.find(spec);
        if (find_pair != type_map.end())
        {
            type = find_pair->second;
        }  
    }

    llvm::Type* codegen(Visitor& visitor);
};

class AstStorageClassSpecifier // : public AstNode
{
    // temporarily not used
};

class AstTypeQualifier //  : public AstNode
{
public:
    // temporarily not used
    std::string qualifier;
};

class AstDirectDeclarator // : public AstNode
{
public:
    std::string id_name;
    AstDirectDeclarator(std::string str): id_name(str) {} 
};

class AstDeclarator // : public AstNode
{
public:
    // pointer is not supported now
    enum class DeclaratorType {POINTER, VAR};

    AstDirectDeclarator* direct_declarator;
    DeclaratorType declarator_type;

    AstDeclarator(AstDirectDeclarator* direct_declarator): direct_declarator(direct_declarator), declarator_type(DeclaratorType::VAR) {}
};

class AstInitializer : public AstNode
{
public:
    AstAssignmentExpr* assignment_expr;

    AstInitializer(AstAssignmentExpr* expr): assignment_expr(expr) {}

    virtual llvm::Value* codegen(Visitor& visitor) override;    
};

class AstInitDeclarator // : public AstNode
{
public:
    AstDeclarator* declarator;
    AstInitializer* initializer;

    AstInitDeclarator(AstDeclarator* decl): declarator(decl), initializer(nullptr) {}
    AstInitDeclarator(AstDeclarator* decl, AstInitializer* init): declarator(decl), initializer(init) {}   
};

class AstInitDeclaratorList // : public AstNode
{
public:
    std::vector<AstInitDeclarator* > init_declarators;

    AstInitDeclaratorList(AstInitDeclarator* decl) { init_declarators.push_back(decl); }
    void add_decl(AstInitDeclarator* decl) { this->init_declarators.push_back(decl); }
};

class AstDeclSpecifiers : public AstNode
{
public:
    std::vector<AstStorageClassSpecifier*> stor_specs;
    std::vector<AstTypeSpecifier*> type_specs;
    std::vector<AstTypeQualifier*> type_quals;

    AstDeclSpecifiers(AstStorageClassSpecifier* stor_spec) { this->stor_specs.push_back(stor_spec); }
    AstDeclSpecifiers(AstTypeSpecifier* type_spec) { this->type_specs.push_back(type_spec); }
    AstDeclSpecifiers(AstTypeQualifier* type_qual) { this->type_quals.push_back(type_qual); }

    void add_stor_spec(AstStorageClassSpecifier* stor_spec) { this->stor_specs.push_back(stor_spec); }
    void add_type_spec(AstTypeSpecifier* type_spec) { this->type_specs.push_back(type_spec); }
    void add_type_qual(AstTypeQualifier* type_qual) { this->type_quals.push_back(type_qual); }
    
    virtual llvm::Value* codegen(Visitor& visitor) override;
};

// create variables here
class AstDecl : public AstStatement
{
public:
    AstDeclSpecifiers* decl_specifiers;
    AstInitDeclaratorList* init_declarator_list;

    AstDecl(AstDeclSpecifiers* decl_spec) : decl_specifiers(decl_spec), init_declarator_list(nullptr) {}
    AstDecl(AstDeclSpecifiers* decl_spec, AstInitDeclaratorList* init_decl_list): decl_specifiers(decl_spec), init_declarator_list(init_decl_list) {}

    virtual llvm::Value* codegen(Visitor& visitor) override;
};

class AstFunctionDef : public AstStatement
{

};

class AstExternDecl : public AstStatement
{
public:
    enum class DeclType {VAR, FUNC};

    DeclType decl_type;
    AstDecl* declaration;
    AstFunctionDef* function_definition;

    AstExternDecl(AstDecl* decl) : declaration(decl), decl_type(DeclType::VAR) {};
    AstExternDecl(AstFunctionDef* function_def) : function_definition(function_def), decl_type(DeclType::FUNC) {}

    virtual llvm::Value* codegen(Visitor& visitor) override;
};

class AstTranslationUnit : public AstNode
{
public:
    std::vector<AstExternDecl*> external_decl_list;

    AstTranslationUnit(AstExternDecl* exdec) { external_decl_list.push_back(exdec); }
    void add_exdec(AstExternDecl* exdec) { external_decl_list.push_back(exdec); }

    virtual llvm::Value* codegen(Visitor& visitor) override;
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
