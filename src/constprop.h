#pragma once

#include "ast.h"

namespace ConstantPropagation
{

class CPVisitor : public ASTVisitorBase
{
public:

    void visit(ASTNode &v) override;
};

};
