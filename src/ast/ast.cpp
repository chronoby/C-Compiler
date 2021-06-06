#include "ast.h"
#include "../visitor/visitor.h"

llvm::Value* AstInt::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstIdentifier::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstAssignment::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstExprStmt::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstVariableDeclaration::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}

llvm::Value* AstBlock::codegen(Visitor& visitor)
{
    return visitor.codegen(*this);
}
