
#include "constprop.h"

void ConstantPropagation::CPVisitor::visit(ASTNode &v)
{
     for(auto child : v.m_children)
     {
         visit(*child.get());
     }

     if (v.m_type == ASTNode::NodeType::ADD)
     {
         if ((v.m_children.at(0)->m_type == ASTNode::NodeType::INTEGER) && 
            (v.m_children.at(1)->m_type == ASTNode::NodeType::INTEGER))
         {
             v.m_intValue = v.m_children.at(0)->m_intValue + v.m_children.at(1)->m_intValue;
            v.m_children.clear();
            v.m_type = ASTNode::NodeType::INTEGER;             
         }
     }
     else if (v.m_type == ASTNode::NodeType::SUB)
     {
         if ((v.m_children.at(0)->m_type == ASTNode::NodeType::INTEGER) && 
            (v.m_children.at(1)->m_type == ASTNode::NodeType::INTEGER))
         {
             v.m_intValue = v.m_children.at(0)->m_intValue - v.m_children.at(1)->m_intValue;
            v.m_children.clear();
            v.m_type = ASTNode::NodeType::INTEGER;             
         }
     }
     else if (v.m_type == ASTNode::NodeType::MUL)
     {
         if ((v.m_children.at(0)->m_type == ASTNode::NodeType::INTEGER) && 
            (v.m_children.at(1)->m_type == ASTNode::NodeType::INTEGER))
         {
             v.m_intValue = v.m_children.at(0)->m_intValue * v.m_children.at(1)->m_intValue;
            v.m_children.clear();
            v.m_type = ASTNode::NodeType::INTEGER;             
         }
     }     
     else if (v.m_type == ASTNode::NodeType::DIV)
     {
         if ((v.m_children.at(0)->m_type == ASTNode::NodeType::INTEGER) && 
            (v.m_children.at(1)->m_type == ASTNode::NodeType::INTEGER))
         {             
             //FIXME: check for divide by zero
             v.m_intValue = v.m_children.at(0)->m_intValue / v.m_children.at(1)->m_intValue;
            v.m_children.clear();
            v.m_type = ASTNode::NodeType::INTEGER;             
         }
     }       
}
