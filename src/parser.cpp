#include "parser.h"

void Parser::setupLexer()
{
    m_lex.addExtendedAlphaChar('_');

    for(const auto &kw : ms_keywords)
    {
        m_lex.registerKeyword(kw.name, kw.id);
    }
}

Parser::ParseResult Parser::parse()
{
    ParseResult result;

    m_token = m_lex.nextToken();
    result = statements();

    return result;
}

bool Parser::match(uint16_t id)
{
    if (static_cast<uint16_t>(m_token.m_type) == id)
    {
        m_matchedToken = m_token;
        m_token = m_lex.nextToken();
        return true;
    }
    return false;
}

bool Parser::match(Lexer::TokenType type)
{
    if (m_token.m_type == type)
    {
        m_matchedToken = m_token;
        m_token = m_lex.nextToken();
        return true;
    }
    return false;
}

Parser::ParseResult Parser::statements()
{
    Parser::ParseResult result;
    result.m_node = std::make_shared<ASTNode>(ASTNode::NodeType::STATEMENTS);

    while(m_token.m_type != Lexer::TokenType::ENDFILE)
    {
        auto parseResult = statement();
        if (!parseResult.m_ok)
        {
            break;    
        }
        
        result.m_node->addChildNode(parseResult.m_node);
    };

    result.m_ok = true;
    return result;
}

Parser::ParseResult Parser::statement()
{
    auto tryAssignment = assignment();
    if (tryAssignment.m_ok)
    {
        return tryAssignment;
    }

    if (match(TOK_FOR))
    {
        // assignment expression
        auto assignResult = assignment();
        if (!assignResult.m_ok)
        {
            error("Expected an assignment in FOR loop");
            return ParseResult::invalid();
        }

        if(!match(TOK_TO))
        {
            error("Expected a TO keyword in FOR loop");
            return ParseResult::invalid();
        }

        auto toResult = expression();
        if (!toResult.m_ok)
        {
            error("Exptected a valid expression after TO");
            return ParseResult::invalid();
        }

        auto block = statements();
        if (!block.m_ok)
        {
            return ParseResult::invalid();
        }

        // create for loop AST node
        auto loopNode = std::make_shared<ASTNode>(ASTNode::NodeType::FORLOOP);

        loopNode->addChildNode(assignResult.m_node);
        loopNode->addChildNode(toResult.m_node);
        loopNode->addChildNode(block.m_node);

        return ParseResult::valid(loopNode);
    }

    return ParseResult::invalid();
}

Parser::ParseResult Parser::assignment()
{
    if (match(Lexer::TokenType::IDENT))
    {
        auto ident = m_matchedToken.m_tokstr;

        // create left side of the assignment tree
        auto leftNode = std::make_shared<ASTNode>(ASTNode::NodeType::VAR);
        leftNode->m_varName  = ident;

        // get right side of the assignment tree
        auto assignResult = assignment_rhs();
        if (!assignResult.m_ok)
        {
            return Parser::ParseResult::invalid();
        }

        // create assignment AST node
        auto assignNode = std::make_shared<ASTNode>(ASTNode::NodeType::ASSIGN);

        assignNode->addChildNode(leftNode);
        assignNode->addChildNode(assignResult.m_node);

        return ParseResult{assignNode, true};
    }
    
    return ParseResult::invalid();
}

Parser::ParseResult Parser::assignment_rhs()
{
    if (!match(Lexer::TokenType::EQUALS))
    {
        error("Assignment expected '='");
        return ParseResult::invalid();
    }

    auto exprResult = expression();

    if (!exprResult.m_ok)
    {
        return ParseResult::invalid();
    }

    return exprResult;
}

Parser::ParseResult Parser::expression()
{
    auto termResult = term();
    
    if (!termResult.m_ok)
    {
        return ParseResult::invalid();
    }

    auto leftNode = termResult.m_node;

    while(match(Lexer::TokenType::PLUS) || match(Lexer::TokenType::MINUS))
    {
        auto op = m_matchedToken.m_tokstr;
        auto opType = m_matchedToken.m_type;

        auto rightTermResults = term();
        if (!rightTermResults.m_ok)
        {
            return ParseResult::invalid();
        }

        //std::cout << "OP: " << op << "\n";

        std::shared_ptr<ASTNode> opNode;
        if (opType == Lexer::TokenType::PLUS)
        {
            opNode = std::make_shared<ASTNode>(ASTNode::NodeType::ADD);
        }
        else
        {
            opNode = std::make_shared<ASTNode>(ASTNode::NodeType::SUB);
        }

        opNode->addChildNode(leftNode);
        opNode->addChildNode(rightTermResults.m_node);
        leftNode = opNode;
    }

    return ParseResult::valid(leftNode);
}

Parser::ParseResult Parser::term()
{
    auto factorResult = factor();

    if (!factorResult.m_ok)
    {
        return ParseResult::invalid();
    }

    auto leftNode = factorResult.m_node;
    while(match(Lexer::TokenType::STAR) || match(Lexer::TokenType::SLASH))
    {
        auto op = m_matchedToken.m_tokstr;
        auto opType = m_matchedToken.m_type;

        auto rightFactorResults = factor();
        if (!rightFactorResults.m_ok)
        {
            return ParseResult::invalid();
        }

        //std::cout << "OP: " << op << "\n";

        std::shared_ptr<ASTNode> opNode;
        if (opType == Lexer::TokenType::STAR)
        {
            opNode = std::make_shared<ASTNode>(ASTNode::NodeType::MUL);
        }
        else
        {
            opNode = std::make_shared<ASTNode>(ASTNode::NodeType::DIV);
        }

        opNode->addChildNode(leftNode);
        opNode->addChildNode(rightFactorResults.m_node);
        leftNode = opNode;
    }

    return ParseResult::valid(leftNode);
}

Parser::ParseResult Parser::factor()
{
    if (match(Lexer::TokenType::LPAREN))
    {
        auto exprResult = expression();
        if (!exprResult.m_ok)
        {
            error("Expected an expression");
            return ParseResult::invalid();
        }
        
        if (!match(Lexer::TokenType::RPAREN))
        {
            error("Expected a closing )");
            return ParseResult::invalid();
        }
        return exprResult;
    }

    if (match(Lexer::TokenType::NUMBER))
    {
        auto numNode = std::make_shared<ASTNode>(ASTNode::NodeType::INTEGER);
        numNode->m_intValue = m_matchedToken.m_integer;
        return ParseResult::valid(numNode);
    }

    if (match(Lexer::TokenType::IDENT))
    {
        auto numNode = std::make_shared<ASTNode>(ASTNode::NodeType::VAR);
        numNode->m_varName = m_matchedToken.m_tokstr;
        return ParseResult::valid(numNode);
    }

    if (match(Lexer::TokenType::MINUS))
    {
        auto unaryNode = std::make_shared<ASTNode>(ASTNode::NodeType::NEG);

        if (!match(Lexer::TokenType::NUMBER))
        {
            error("Expected number after unary minus");                
        }
        
        auto numNode = std::make_shared<ASTNode>(ASTNode::NodeType::INTEGER);
        numNode->m_intValue = m_matchedToken.m_integer;

        unaryNode->addChildNode(numNode);

        //std::cout << "VAR: " << m_matchedToken.m_tokstr << "\n";
        //std::cout << "- (unary)\n";
        return ParseResult::valid(unaryNode);
    }

    error("Invalid factor");
    return ParseResult::invalid();
}

void Parser::error(const char *txt)
{
    std::cout << "Error: " << txt << "  line: " << m_token.m_pos.m_line << " col:" << m_token.m_pos.m_col << "\n";
}
