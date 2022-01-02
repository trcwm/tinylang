#include "ast.h"

void ASTNode::dump(std::ostream &os, size_t depth) const noexcept
{
    for(size_t i=0; i<depth; i++)
    {
        os << "  ";
    }

    switch(m_type)
    {
    case NodeType::ADD:
        os << "ADD\n";
        break;
    case NodeType::SUB:
        os << "SUB\n";
        break;
    case NodeType::ASSIGN:
        os << "ASSIGN " << m_varName << "\n";
        break;
    case NodeType::INTEGER:
        os << "INT " << m_intValue << "\n";
        break;
    case NodeType::VARDECL:
        os << "VARDECL " << m_varName << "  type: " << static_cast<int>(m_symType) << "\n";
        break;                
    case NodeType::VAR:
        os << "VAR " << m_varName << "\n";
        break;        
    case NodeType::MUL:
        os << "MUL\n";
        break;    
    case NodeType::DIV:
        os << "DIV\n";
        break;
    case NodeType::JNE:
        os << "JNE LABEL #" << m_intValue << "\n";
        break;
    case NodeType::JMP:
        os << "JMP LABEL #" << m_intValue << "\n";
        break;                
    case NodeType::STATEMENTS:
        os << "STATEMENTS\n";
        break;
    case NodeType::NEG:
        os << "NEG\n";
        break;
    case NodeType::LABEL:
        os << "LABEL #" << m_intValue << "\n";
        break;        
    case NodeType::UNKNOWN:
        os << "???\n";
        break;    
    case NodeType::TOP:
        os << "TOP\n";
        break;        
    case NodeType::FORLOOP:
        os << "FORLOOP\n";
        break;        
    case NodeType::IF:
        os << "IF\n";
        break;   
    case NodeType::COMMENT:
        os << "COMMENT: " << m_comment << "\n";
        break;       
    case NodeType::DEALLOC:
        os << "DEALLOC " << m_intValue << "\n";
        break;           
    }

    for(auto const& child : m_children)
    {
        child->dump(os, depth+1);
    }
}
