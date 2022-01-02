#pragma once
#include <memory>
#include <sstream>

#include "mytypes.h"
#include "symboltable.h"
#include "ast.h"

class IRGenVisitor : public ASTVisitorBase
{
public:
    void visit(const ASTNode &v) override;

    std::string getErrors() const
    {
        return m_errors.str();
    }

    std::string getCode() const
    {
        return m_code.str();
    }

    constexpr bool hasErrors() const noexcept
    { 
        return m_hasErrors;
    }

protected:
    void error(const std::string &txt, const LinePos &pos)
    {
        m_errors << "Error: " << pos.m_line << "," << pos.m_col << " " << txt << "\n";
        m_hasErrors = true;
    }

    bool                m_hasErrors = false;
    std::stringstream   m_errors;
    std::stringstream   m_code;
    SymbolTable         m_symTable;
};

