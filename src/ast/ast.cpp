#include "ast.h"
#include "../visitor/visitor.h"

// --------------------------- expression -------------------------

llvm::Value* AstPrimaryExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstPostfixExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstUnaryExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstCastExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstMultiplicativeExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstAdditiveExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstShiftExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstRelationalExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstEqualityExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstAndExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstExclusiveExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstInclusiveExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstLogicalAndExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstLogicalOrExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstConditionalExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstAssignmentExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstExpr::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

// ---------------------- DECLRATION -----------------------------

llvm::Value* AstExternDecl::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstDecl::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Type* AstTypeSpecifier::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstDeclSpecifiers::codegen(Visitor& visitor)
{
    // do nothing, temporarily
    return nullptr;
}

llvm::Value* AstInitializer::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstStmt::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstCompoundStmt::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstTranslationUnit::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstFunctionDef::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstStmtList::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstDeclList::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

// ----------------------------------------------------------------