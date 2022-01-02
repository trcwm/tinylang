#pragma once
#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <cstdint>
#include <iostream>

#include "mytypes.h"

class ASTNode; //predeclaration

class ASTVisitorBase
{
public:
    virtual ~ASTVisitorBase() = default;
    
    virtual void visit(ASTNode &v) {};
    virtual void visit(const ASTNode &v) {};
};

class ASTNode
{
public:
    enum class NodeType : int16_t
    {
        UNKNOWN = 0,
        TOP,
        ADD,
        SUB,
        MUL,
        DIV,
        NEG,
        VARDECL,
        VAR,
        FORLOOP,
        INTEGER,
        ASSIGN,
        STATEMENTS
    };

    explicit ASTNode(NodeType mytype) : m_type(mytype) {}

    virtual void accept(ASTVisitorBase& v) 
    {
        v.visit(*this);
    }

    virtual void accept(ASTVisitorBase& v) const
    {
        v.visit(*this);
    }

    void addChildNode(std::shared_ptr<ASTNode> node)
    {
        m_children.push_back(node);
    }

    ASTNode* child(size_t idx) const noexcept
    {
        return m_children.at(idx).get();
    }

    std::vector<std::shared_ptr<ASTNode> > m_children;
    
    void dump(std::ostream &os, size_t depth = 0) const noexcept;

    NodeType        m_type;
    int16_t         m_intValue;     ///< variable value (when type = INTEGER)
    std::string     m_varName;      ///< variable name (when type = VAR)
    LinePos         m_pos;          ///< line position where this node originated
    SymbolType      m_symType;      ///< symbolt type (when type = VARDECL)
};
