#include "irgen.h"

void IRGenVisitor::visit(const ASTNode &v)
{
    for(auto const &child : v.m_children)
    {
        visit(*child.get());
    }

    switch(v.m_type)
    {
    default:
        break;
    case ASTNode::NodeType::ADD:
        m_code << "ADD\n";
        break;
    case ASTNode::NodeType::SUB:
        m_code << "SUB\n";
        break;        
    case ASTNode::NodeType::DIV:
        m_code << "DIV\n";
        break;        
    case ASTNode::NodeType::MUL:
        m_code << "MUL\n";
        break;
    case ASTNode::NodeType::INTEGER:
        m_code << "LOADINT " << v.m_intValue << "\n";
        break;
    case ASTNode::NodeType::VARDECL:
        m_symTable.addSymbol(v.m_varName, v.m_symType);
        m_code << "ALLOC 2\t\t;" << v.m_varName << "\n";
        break;
    case ASTNode::NodeType::LABEL:
        m_code << "@" << v.m_intValue << "\n";
        break;
    case ASTNode::NodeType::JNE:
        m_code << "JNE @" << v.m_intValue << "\n";
        break;        
    case ASTNode::NodeType::ASSIGN:
        {
            auto *sym = m_symTable.getSymbol(v.m_varName);
            if (sym == nullptr)
            {
                std::stringstream ss;
                ss << "Unknown variable " << v.m_varName;
                error(ss.str(), v.m_pos);
            }

            m_code << "STORE " << sym->m_offset << "\t\t;" << v.m_varName << "\n";
        }
        break;
    case ASTNode::NodeType::COMMENT:\
        m_code << "; " << v.m_comment << "\n";
        break;
    case ASTNode::NodeType::VAR:
        {
            auto *sym = m_symTable.getSymbol(v.m_varName);
            if (sym == nullptr)
            {
                std::stringstream ss;
                ss << "Unknown variable " << v.m_varName;
                error(ss.str(), v.m_pos);
                return;
            }

            m_code << "LOAD " << sym->m_offset  << "\t\t;" << v.m_varName << "\n";
        }
        break;
    case ASTNode::NodeType::DEALLOC:
        m_code << "DEALLOC " << v.m_intValue;
        if (!v.m_varName.empty())
        {
            m_code << "\t\t; " << v.m_varName;
        }
        m_code << "\n";
        break;        
    case ASTNode::NodeType::FORLOOP:
        break;
    }
}
