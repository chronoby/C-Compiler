#include "ast.h"
#include "../visitor/visitor.h"

// --------------------------- expression -------------------------

std::shared_ptr<Variable> AstPrimaryExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstPostfixExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstUnaryExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstCastExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstMultiplicativeExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstAdditiveExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstShiftExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstRelationalExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstEqualityExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstAndExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstExclusiveExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstInclusiveExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstLogicalAndExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstLogicalOrExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstConditionalExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstAssignmentExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

// ---------------------- DECLRATION -----------------------------

std::shared_ptr<Variable> AstExternDecl::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstDecl::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Type* AstTypeSpecifier::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstDeclSpecifiers::codegen(Visitor& visitor)
{
    // do nothing, temporarily
    return nullptr;
}

std::shared_ptr<Variable> AstInitializer::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstStmt::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstCompoundStmt::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstTranslationUnit::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstFunctionDef::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstStmtList::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

std::shared_ptr<Variable> AstDeclList::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

// ----------------------------------------------------------------

std::shared_ptr<Variable> AstExprStmt::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}