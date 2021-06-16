#ifndef AST_H_
#define AST_H_

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <llvm/IR/Value.h>

class Visitor;
class Variable;

class PosEntity
{
private:
    int line;
    int column;
public:
    PosEntity(): line(0), column(0) {}
    void setPosition(int l, int c) {line = l; column = c;}
    void errorMsg(std::string msg) const;
    void warningMsg(std::string msg) const;
};

class AstNode
{
public:
    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) = 0;
};

class AstExpression : public AstNode { };
class AstStatement : public AstNode { };

class AstPrimaryExpr;
class AstPostfixExpr;
class AstArgumentExprList;
class AstUnaryExpr;
class AstUnaryOp;
class AstCastExpr;
class AstMultiplicativeExpr;
class AstAdditiveExpr;
class AstShiftExpr;
class AstRelationalExpr;
class AstEqualityExpr;
class AstAndExpr;
class AstExclusiveExpr;
class AstInclusiveExpr;
class AstLogicalAndExpr;
class AstLogicalOrExpr;
class AstConditionalExpr;
class AstAssignmentExpr;
class AstExpr;

class AstDecl;
class AstDeclSpecifiers;
class AstInitDeclaratorList;
class AstInitDeclarator;
class AstStorageClassSpecifier;
class AstTypeSpecifier;
class AstTypeQualifier;
class AstDeclarator;
class AstDirectDeclarator;
class AstPointer;
class AstTypeQualifierList;
class AstParameterTypeList;
class AstParameterList;
class AstParameterDecl;
class AstAbstractDeclarator;
class AstInitializer;
class AstInitializerList;
class AstStmt;
class AstCompoundStmt;
class AstSelectionStmt;
class AstIterStmt;
class AstDeclList;
class AstStmtList;
class AstExprStmt;
class AstJumpStmt;
class AstTranslationUnit;
class AstExternDecl;
class AstFunctionDef;

// --------------------------- expression -------------------------

class AstArgumentExprList : public PosEntity // : public AstExpression
{
public:
    std::vector<AstAssignmentExpr*> expr_list;
    AstArgumentExprList(AstAssignmentExpr* assign_expr) { expr_list.push_back(assign_expr); }
    void add_expr(AstAssignmentExpr* assign_expr) { expr_list.push_back(assign_expr); }
};

// Type for primary_expr
class AstPrimaryExpr : public AstExpression, public PosEntity
{
public:
    // which grammar rule is used to derive this node
    enum class ExprType {ID, CONSTANT, PR_EXPR};
    
    // type of data if it is a constant
    enum class DataType {INTEGER, HEXI, OCTAL, FLOAT, CHAR, STRING};
    
    AstPrimaryExpr(AstPrimaryExpr::DataType dtype, std::string val): value(val), data_type(dtype), expr_type(AstPrimaryExpr::ExprType::CONSTANT) {}
    // AstPrimaryExpr(AstPrimaryExpr::DataType dtype, char* val): str_value(val), data_type(dtype), expr_type(AstPrimaryExpr::ExprType::CONSTANT) {}
    AstPrimaryExpr(std::string name): identifier_name(name), expr_type(AstPrimaryExpr::ExprType::ID) {}
    AstPrimaryExpr(AstExpr* expr): expr(expr), value(""), expr_type(AstPrimaryExpr::ExprType::PR_EXPR) {}

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
// private:
    // which grammar rule is used to derive this node
    ExprType expr_type;

    // '('expression')'
    AstExpr* expr;

    // INTEGER | HEXI | OCTAL | ...
    DataType data_type;
    std::string value;
    char* str_value;

    std::string identifier_name;
};

// NOTE: TO BE FINISHED
class AstPostfixExpr : public AstExpression, public PosEntity
{
public:
    // which grammar rule is used to derive this node
    enum class ExprType {PRIMARY, IDX, FUNC, FUNC_PARAM, MEMBER, PTR_MEMBER, OP};
    
    // in term of a++ or a--
    enum class OpType {INC, DEC};
    
    // TO BE FINISED
    AstPostfixExpr(AstPrimaryExpr* primary_expr): primary_expr(primary_expr), expr_type(ExprType::PRIMARY), identifier_name(primary_expr->identifier_name) {};
    AstPostfixExpr(AstPostfixExpr* expr, AstExpr* ind): postfix_expr(expr), expr(ind), expr_type(ExprType::IDX) { }
    // AstPostfixExpr(AstPostfixExpr* expr, OpType op): postfix_expr(expr), op_type(op), expr_type(ExprType::OP) { }
    // AstPostfixExpr(AstPostfixExpr* expr, ExprType expr_type): postfix_expr(expr), expr_type(expr_type) {}
    AstPostfixExpr(AstPostfixExpr* expr): postfix_expr(expr) {}
    void setExprType(ExprType expr_type) { this->expr_type = expr_type; }
    void setOpType(OpType op_type) { this->op_type = op_type; }
    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
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
};

class AstUnaryExpr : public AstExpression, public PosEntity
{
public:
    enum class ExprType {POSTFIX, OP, UNARY_OP, CAST, SIZEOF_TYPE, SIZEOF_EXPR};
    enum class OpType {INC, DEC};

    AstUnaryExpr(AstPostfixExpr* expr): postfix_expr(expr), expr_type(ExprType::POSTFIX) { }
    AstUnaryExpr(AstUnaryExpr* expr, AstUnaryExpr::OpType type): unary_expr(expr), expr_type(ExprType::OP), op_type(type) { }
    AstUnaryExpr(AstCastExpr* expr, AstUnaryOp* unary_op): cast_expr(expr), unary_op(unary_op), expr_type(ExprType::UNARY_OP) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;

// private:
    AstPostfixExpr* postfix_expr;
    AstCastExpr* cast_expr;
    AstUnaryExpr* unary_expr;
    AstUnaryOp* unary_op;
    ExprType expr_type;
    OpType op_type;
};

class AstUnaryOp : public PosEntity
{
public:
    enum class OpType
    {
        AND,
        STAR,
        PLUS,
        MINUS,
        INV,
        NOT
    };

    OpType type;
    AstUnaryOp(OpType t): type(t) {}
};

class AstCastExpr: public AstExpression, public PosEntity
{
public:
    enum class ExprType {UNARY, CAST};

    AstCastExpr(AstUnaryExpr* expr): unary_expr(expr), expr_type(ExprType::UNARY) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstUnaryExpr* unary_expr;
    AstCastExpr* cast_expr;
};

class AstMultiplicativeExpr: public AstExpression, public PosEntity
{
public:
    enum class ExprType {CAST, OP};
    enum class OpType {MUL, DIV, MOD};

    AstMultiplicativeExpr(AstCastExpr* expr): cast_expr(expr), expr_type(ExprType::CAST) { }
    AstMultiplicativeExpr(AstMultiplicativeExpr* m_expr, OpType op, AstCastExpr* expr): 
        multi_expr(m_expr), op_type(op), cast_expr(expr), expr_type(ExprType::OP) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    OpType op_type;
    AstCastExpr* cast_expr;
    AstMultiplicativeExpr* multi_expr;
};

class AstAdditiveExpr: public AstExpression, public PosEntity
{
public:
    enum class ExprType {MULTI, OP};
    enum class OpType {PLUS, MINUS};

    AstAdditiveExpr(AstMultiplicativeExpr* expr): multi_expr(expr), expr_type(ExprType::MULTI) { }
    AstAdditiveExpr(AstAdditiveExpr* a_expr, OpType op, AstMultiplicativeExpr* expr): 
        add_expr(a_expr), op_type(op), multi_expr(expr), expr_type(ExprType::OP) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    OpType op_type;
    AstMultiplicativeExpr* multi_expr;
    AstAdditiveExpr* add_expr;
};

class AstShiftExpr: public AstExpression, public PosEntity
{
public:
    enum class ExprType {ADD, OP};
    enum class OpType {LEFT, RIGHT};

    AstShiftExpr(AstAdditiveExpr* expr): add_expr(expr), expr_type(ExprType::ADD) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstAdditiveExpr* add_expr;
};

class AstRelationalExpr: public AstExpression, public PosEntity
{
public:
    enum class ExprType {SHIFT, OP};
    enum class OpType {LESS, GREATER, LE, GE};

    AstRelationalExpr(AstShiftExpr* expr): shift_expr(expr), expr_type(ExprType::SHIFT) { }
    AstRelationalExpr(AstRelationalExpr* r_expr, OpType op, AstShiftExpr* expr): 
        rela_expr(r_expr), op_type(op), shift_expr(expr), expr_type(ExprType::OP) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    OpType op_type;
    AstShiftExpr* shift_expr;
    AstRelationalExpr* rela_expr;
};

class AstEqualityExpr: public AstExpression, public PosEntity
{
public:
    enum class ExprType {RELATIONAL, OP};
    enum class OpType {EQ, NE};

    AstEqualityExpr(AstRelationalExpr* expr): rela_expr(expr), expr_type(ExprType::RELATIONAL) { }
    AstEqualityExpr(AstEqualityExpr* e_expr, OpType op, AstRelationalExpr* expr): 
        equal_expr(e_expr), op_type(op), rela_expr(expr), expr_type(ExprType::OP) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    OpType op_type;
    AstRelationalExpr* rela_expr;
    AstEqualityExpr* equal_expr;
};

class AstAndExpr: public AstExpression, public PosEntity
{
public:
    enum class ExprType {EQUALITY, OP};

    AstAndExpr(AstEqualityExpr* expr): equal_expr(expr), expr_type(ExprType::EQUALITY) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstEqualityExpr* equal_expr;
};

class AstExclusiveExpr: public AstExpression, public PosEntity
{
public:
    enum class ExprType {AND, OP};

    AstExclusiveExpr(AstAndExpr* expr): and_expr(expr), expr_type(ExprType::AND) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstAndExpr* and_expr;
};

class AstInclusiveExpr: public AstExpression, public PosEntity
{
public:
    enum class ExprType {EXCLUSIVE, OP};
    AstInclusiveExpr(AstExclusiveExpr* expr): exclusive_expr(expr), expr_type(ExprType::EXCLUSIVE) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstExclusiveExpr* exclusive_expr;
};

class AstLogicalAndExpr: public AstExpression, public PosEntity
{
public:
    enum class ExprType {INCLUSIVE, OP};
    AstLogicalAndExpr(AstInclusiveExpr* expr): inclusive_expr(expr), expr_type(ExprType::INCLUSIVE) { }
    AstLogicalAndExpr(AstLogicalAndExpr* a_expr, AstInclusiveExpr* expr): 
        and_expr(a_expr), inclusive_expr(expr), expr_type(ExprType::OP) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstInclusiveExpr* inclusive_expr;
    AstLogicalAndExpr* and_expr;
};

class AstLogicalOrExpr: public AstExpression, public PosEntity
{
public:
    enum class ExprType {LOG_AND, OP};

    AstLogicalOrExpr(AstLogicalAndExpr* expr): and_expr(expr), expr_type(ExprType::LOG_AND) { }
    AstLogicalOrExpr(AstLogicalOrExpr* o_expr, AstLogicalAndExpr* expr): 
        or_expr(o_expr), and_expr(expr), expr_type(ExprType::OP) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstLogicalAndExpr* and_expr;
    AstLogicalOrExpr* or_expr;
};

class AstConditionalExpr: public AstExpression, public PosEntity
{
public:
    enum class ExprType {LOG_OR, OP};

    AstConditionalExpr(AstLogicalOrExpr* expr): or_expr(expr), expr_type(ExprType::LOG_OR) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstLogicalOrExpr* or_expr;
};

class AstAssignmentExpr: public AstExpression, public PosEntity
{
public:
    enum class ExprType {CONDITIONAL, ASSIGN};

    AstAssignmentExpr(AstConditionalExpr* expr): cond_expr(expr), expr_type(ExprType::CONDITIONAL) { }
    AstAssignmentExpr(AstUnaryExpr* expr, AstAssignmentExpr* a_expr): unary_expr(expr), assign_expr(a_expr),
        expr_type(ExprType::ASSIGN) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstConditionalExpr* cond_expr;
    AstUnaryExpr* unary_expr;
    AstAssignmentExpr* assign_expr;
};

class AstExpr: public AstExpression, public PosEntity
{
public:
    enum class ExprType {ASSIGN, OP};

    AstExpr(AstAssignmentExpr* expr): assign_expr(expr), expr_type(ExprType::ASSIGN) { }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
    
    ExprType expr_type;
    AstAssignmentExpr* assign_expr;
};

// ---------------------- DECLRATION ---------------------------

// create variables here
class AstDecl  : public AstStatement, public PosEntity
{
public:
    AstDeclSpecifiers* decl_specifiers;
    AstInitDeclaratorList* init_declarator_list;

    AstDecl(AstDeclSpecifiers* decl_spec) : decl_specifiers(decl_spec), init_declarator_list(nullptr) {}
    AstDecl(AstDeclSpecifiers* decl_spec, AstInitDeclaratorList* init_decl_list): decl_specifiers(decl_spec), init_declarator_list(init_decl_list) {}

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
};

class AstDeclSpecifiers : public AstNode, public PosEntity
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
    
    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
};

class AstInitDeclaratorList : public PosEntity // : public AstNode
{
public:
    std::vector<AstInitDeclarator* > init_declarators;

    AstInitDeclaratorList(AstInitDeclarator* decl) { init_declarators.push_back(decl); }
    void add_decl(AstInitDeclarator* decl) { this->init_declarators.push_back(decl); }
};

class AstInitDeclarator : public PosEntity // : public AstNode
{
public:
    AstDeclarator* declarator;
    AstInitializer* initializer;

    AstInitDeclarator(AstDeclarator* decl): declarator(decl), initializer(nullptr) {}
    AstInitDeclarator(AstDeclarator* decl, AstInitializer* init): declarator(decl), initializer(init) {}   
};

class AstStorageClassSpecifier : public PosEntity // : public AstNode
{
    // temporarily not used
};

class AstTypeSpecifier : public PosEntity
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

class AstTypeQualifier : public PosEntity //  : public AstNode
{
public:
    // temporarily not used
    std::string qualifier;
};

class AstDeclarator : public PosEntity // : public AstNode
{
public:
    // pointer is not supported now
    enum class DeclaratorType {POINTER, VAR};

    AstPointer* pointer;
    AstDirectDeclarator* direct_declarator;
    DeclaratorType declarator_type;

    AstDeclarator(AstDirectDeclarator* direct_declarator): direct_declarator(direct_declarator), pointer(nullptr), declarator_type(DeclaratorType::VAR) {}
    AstDeclarator(AstPointer* pointer, AstDirectDeclarator* direct_declarator): pointer(pointer), direct_declarator(direct_declarator), declarator_type(DeclaratorType::POINTER) {}
};

class AstDirectDeclarator : public PosEntity // : public AstNode
{
public:
    enum class DeclaratorType {ID, PR, BR, BR_EMPTY, FUNC_PARAM, FUNC_ID, FUNC_EMPTY};
    DeclaratorType declarator_type;
    std::string id_name;
    AstParameterTypeList* param_type_list;
    AstPrimaryExpr* prime_expr;

    AstDirectDeclarator(std::string str, DeclaratorType type): id_name(str), declarator_type(type) {}
    void setType(DeclaratorType type) { this->declarator_type = type; }
};

class AstPointer : public PosEntity
{
public:
    // TODO: ADD CONST IF AVALABLE
    AstPointer* next;
    AstTypeQualifierList* type_qualifier_list;

    AstPointer() : next(nullptr), type_qualifier_list(nullptr) {}
    AstPointer(AstPointer* next): next(next), type_qualifier_list(nullptr) {}
};

class AstTypeQualifierList
{

};

class AstParameterTypeList : public PosEntity
{
public:
    AstParameterList* param_list;
    bool isVarArg;

    AstParameterTypeList(AstParameterList* list, bool isVarArg): param_list(list), isVarArg(isVarArg) {};
};

class AstParameterList : public PosEntity
{
public:
    std::vector<AstParameterDecl*> parameter_list;

    AstParameterList(AstParameterDecl* parameter_decl) { parameter_list.push_back(parameter_decl); }
    void add_param_decl(AstParameterDecl* parameter_decl) { parameter_list.push_back(parameter_decl); }
};

class AstParameterDecl : public PosEntity
{
public:
    AstDeclSpecifiers* decl_specifiers;
    AstDeclarator* declarator;
    AstAbstractDeclarator* abstract_declarator;

    AstParameterDecl(AstDeclSpecifiers* decl_specs, AstDeclarator* decl) : decl_specifiers(decl_specs), declarator(decl), abstract_declarator(nullptr) {}
    AstParameterDecl(AstDeclSpecifiers* decl_specs, AstAbstractDeclarator* abst_decl) : decl_specifiers(decl_specs), declarator(nullptr), abstract_declarator(abst_decl) {}
    AstParameterDecl(AstDeclSpecifiers* decl_specs) : decl_specifiers(decl_specs), declarator(nullptr), abstract_declarator(nullptr) {}
};

class AstAbstractDeclarator : public PosEntity
{
public:
};

class AstInitializer : public AstNode , public PosEntity
{
public:
    enum class InitType {ASSIGN, LIST};

    AstAssignmentExpr* assignment_expr;
    AstInitializerList* initializer_list;
    InitType init_type;

    AstInitializer(AstAssignmentExpr* expr): assignment_expr(expr), init_type(InitType::ASSIGN) {}
    AstInitializer(AstInitializerList* expr): initializer_list(expr), init_type(InitType::LIST) {}

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;    
};

class AstInitializerList// : public AstNode
{
public:
    std::vector<AstInitializer *> initializer_list;

    AstInitializerList(AstInitializer* expr) { initializer_list.push_back(expr); }

};

class AstStmt : public AstNode , public PosEntity
{
public:
    enum class StmtType {LABELED, COMPOUND, EXPR, SELECT, ITER, JUMP};

    StmtType stmt_type;
    AstCompoundStmt* compound_stmt;
    AstExprStmt* expr_stmt;
    AstJumpStmt* jump_stmt;
    AstSelectionStmt* selection_stmt;

    AstStmt(AstCompoundStmt* compound_stmt): compound_stmt(compound_stmt), stmt_type(StmtType::COMPOUND) {}
    AstStmt(AstExprStmt* expr_stmt): expr_stmt(expr_stmt), stmt_type(StmtType::EXPR) {}
    AstStmt(AstSelectionStmt* sele_stmt): selection_stmt(sele_stmt), stmt_type(StmtType::SELECT) {}
    AstStmt(AstJumpStmt* jump_stmt): jump_stmt(jump_stmt), stmt_type(StmtType::JUMP) {}

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
};

class AstCompoundStmt : public AstStatement , public PosEntity
{
public:
    AstStmtList* stmt_list;
    AstDeclList* decl_list;

    AstCompoundStmt() : stmt_list(nullptr), decl_list(nullptr) {} 
    AstCompoundStmt(AstStmtList* stmt_l) : stmt_list(stmt_l), decl_list(nullptr) {}
    AstCompoundStmt(AstDeclList* decl_l) : stmt_list(nullptr), decl_list(decl_l) {}
    AstCompoundStmt(AstDeclList* decl_l, AstStmtList* stmt_l) : stmt_list(stmt_l), decl_list(decl_l) {}

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor);
};

class AstSelectionStmt : public AstStatement, public PosEntity
{
public:
    enum class StmtType {IF, IF_ELSE};
    AstSelectionStmt(AstExpr* ex, AstStmt* stm1) : expr(ex), stmt1(stm1), stmt_type(StmtType::IF) {}
    AstSelectionStmt(AstExpr* ex, AstStmt* stm1, AstStmt* stm2) : 
        expr(ex), stmt1(stm1), stmt2(stm2), stmt_type(StmtType::IF_ELSE) {}

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor);

    StmtType stmt_type;
    AstExpr* expr;
    AstStmt* stmt1;
    AstStmt* stmt2;
};

class AstIterStmt : public AstStatement, public PosEntity
{
public:
    enum class StmtType {WHILE, FOR, DO_WHILE};
    AstIterStmt(AstExpr* ex, AstStmt* stm) : expr(ex), stmt(stm), stmt_type(StmtType::WHILE) {}

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor);

    StmtType stmt_type;
    AstExpr* expr;
    AstStmt* stmt;
};

class AstDeclList: public AstNode, public PosEntity
{
public:
    std::vector<AstDecl*> decls;

    AstDeclList(AstDecl* decl) { decls.push_back(decl); }
    void add_decl(AstDecl* decl) { decls.push_back(decl); }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
};

class AstStmtList : public AstNode, public PosEntity
{
public:
    std::vector<AstStmt*> stmts;

    AstStmtList(AstStmt* stmt) { stmts.push_back(stmt); }
    void add_stmt(AstStmt* stmt) { stmts.push_back(stmt); }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
};

class AstExprStmt  : public AstNode, public PosEntity
{
public:
    AstExpr* expr;
    AstExprStmt() : expr(nullptr) {}
    AstExprStmt(AstExpr* e): expr(e) {}

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
};

class AstJumpStmt : public AstNode, public PosEntity
{
public:
    enum class StmtType {RETURN, RETURN_VALUE, CONTINUE, BREAK};
    StmtType stmt_type;
    AstExpr* expr;

    AstJumpStmt(StmtType t): stmt_type(t) {}
    AstJumpStmt(AstExpr* expr): stmt_type(StmtType::RETURN_VALUE), expr(expr) {}

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
};

class AstTranslationUnit : public AstNode, public PosEntity
{
public:
    std::vector<AstExternDecl*> external_decl_list;

    AstTranslationUnit(AstExternDecl* exdec) { external_decl_list.push_back(exdec); }
    void add_exdec(AstExternDecl* exdec) { external_decl_list.push_back(exdec); }

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
};

class AstExternDecl : public AstStatement, public PosEntity
{
public:
    enum class DeclType {VAR, FUNC};

    DeclType decl_type;
    AstDecl* declaration;
    AstFunctionDef* function_definition;

    AstExternDecl(AstDecl* decl) : declaration(decl), decl_type(DeclType::VAR) {};
    AstExternDecl(AstFunctionDef* function_def) : function_definition(function_def), decl_type(DeclType::FUNC) {}

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override;
};

class AstFunctionDef : public AstStatement, public PosEntity
{
public:
    AstDeclSpecifiers* decl_specifiers;
    AstDeclarator* declarator;
    AstDeclList* decl_list;
    AstCompoundStmt* compound_stmt;

    AstFunctionDef(AstDeclSpecifiers* decl_specs, AstDeclarator* decl, AstDeclList* decl_l, AstCompoundStmt* stmt) :
        decl_specifiers(decl_specs),
        declarator(decl),
        decl_list(decl_l),
        compound_stmt(stmt) {}

    AstFunctionDef(AstDeclSpecifiers* decl_specs, AstDeclarator* decl, AstCompoundStmt* stmt) :
        decl_specifiers(decl_specs),
        declarator(decl),
        decl_list(nullptr),
        compound_stmt(stmt) {}    

    AstFunctionDef(AstDeclarator* decl, AstDeclList* decl_l, AstCompoundStmt* stmt) :
        decl_specifiers(nullptr),
        declarator(decl),
        decl_list(decl_l),
        compound_stmt(stmt) {}

    AstFunctionDef(AstDeclarator* decl, AstCompoundStmt* stmt) :
        decl_specifiers(nullptr),
        declarator(decl),
        decl_list(nullptr),
        compound_stmt(stmt) {}

    virtual std::shared_ptr<Variable> codegen(Visitor& visitor) override; 
};

// ----------------------------------------------------------------
#endif // AST_H_
