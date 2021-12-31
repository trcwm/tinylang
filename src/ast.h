#pragma once
#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <cstdint>
#include <iostream>

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
        VAR,
        FORLOOP,
        INTEGER,
        ASSIGN,
        STATEMENTS
    };

    explicit ASTNode(NodeType mytype) : m_type(mytype) {}

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
    int16_t         m_intValue;
    std::string     m_varName;
};
