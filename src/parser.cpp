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

bool Parser::checkToken(Lexer::TokenType type)
{
    if (m_token.m_type == type)
    {
        return true;
    }
    return false;
}

bool Parser::match(uint16_t id)
{    
    if (m_token.m_keyword == id)
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
    // note: a statements block may be empty
    //       so we should return invalid
    //       if things fail

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

    auto tryFor = forStatement();
    if (tryFor.m_ok)
    {
        return tryFor;
    }

    auto tryVar = varStatement();
    if (tryVar.m_ok)
    {
        return tryVar;
    }

    auto tryIf = ifStatement();
    if (tryIf.m_ok)
    {
        return tryIf;
    }

    return ParseResult::invalid();
}

Parser::ParseResult Parser::assignment()
{
    if (match(Lexer::TokenType::IDENT))
    {
        auto ident = m_matchedToken.m_tokstr;
#if 0
        // create left side of the assignment tree
        auto leftNode = std::make_shared<ASTNode>(ASTNode::NodeType::VAR);
        leftNode->m_varName  = ident;
#endif
        // get right side of the assignment tree
        auto assignResult = assignment_rhs();
        if (!assignResult.m_ok)
        {
            return Parser::ParseResult::invalid();
        }

        // create assignment AST node
        auto assignNode = std::make_shared<ASTNode>(ASTNode::NodeType::ASSIGN);
        assignNode->m_varName = ident;
        assignNode->addChildNode(assignResult.m_node);

        return ParseResult{assignNode, true};
    }
    
    return ParseResult::invalid();
}

Parser::ParseResult Parser::forStatement()
{
    if (match(TOK_FOR))
    {
        auto varDeclNode = std::make_shared<ASTNode>(ASTNode::NodeType::VARDECL);
        
        // peek at the variable and make sure it is in the symbol table
        if (!checkToken(Lexer::TokenType::IDENT))
        {            
            error("Expected an identifier in FOR loop");
            return ParseResult::invalid();            
        }

        auto loopVarName = m_token.m_tokstr;
        varDeclNode->m_varName = loopVarName;
        varDeclNode->m_symType = SymbolType::INTEGER;   

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

        if (!match(TOK_ENDFOR))
        {
            error("FOR loop is missing ENDFOR");
            return ParseResult::invalid();
        }

        // create for loop AST node
        auto loopNode = std::make_shared<ASTNode>(ASTNode::NodeType::FORLOOP);

        loopNode->addCommentNode("FOR loop start");
        loopNode->addChildNode(varDeclNode);

        // load loop variable with starting state
        loopNode->addChildNode(assignResult.m_node);

        auto labelId = m_labelCount++;
        loopNode->addLabelNode(labelId);

        // run the inside block of the loop        
        loopNode->addCommentNode("FOR inner block");
        loopNode->addChildNode(block.m_node);

        // increment the loop variable
        loopNode->addCommentNode("FOR increment loopvar");
        ASTNode varNode(ASTNode::NodeType::VAR);
        varNode.m_varName = loopVarName;
        ASTNode oneNode(ASTNode::NodeType::INTEGER);
        oneNode.m_intValue = 1;
        oneNode.m_symType = SymbolType::INTEGER;
        ASTNode addNode(ASTNode::NodeType::ADD);
        addNode.addChildNode(varNode);
        addNode.addChildNode(oneNode);

        loopNode->addChildNode(addNode);

        // store the loop variable
        ASTNode storeNode(ASTNode::NodeType::ASSIGN);
        storeNode.m_varName = loopVarName;
        storeNode.m_symType = SymbolType::INTEGER;
        loopNode->addChildNode(storeNode);

        // load the loop variable via VAR <loop var>
        // substract the loop variable from the loop end expression using SUB
        loopNode->addCommentNode("FOR check exit condition");
        ASTNode subNode(ASTNode::NodeType::SUB);
        subNode.addChildNode(varNode);
        subNode.addChildNode(toResult.m_node); // to expression

        loopNode->addChildNode(subNode);

        //compare and jump if not equal using: JNE label
        ASTNode jneNode(ASTNode::NodeType::JNE);
        jneNode.m_intValue = labelId;
        loopNode->addChildNode(jneNode);

        // deallocate loop variable
        ASTNode deallocNode(ASTNode::NodeType::DEALLOC);
        
        //FIXME: 2 is for INTEGER, need to change that
        //       once we allow other types
        deallocNode.m_intValue = 2;
        deallocNode.m_varName  = loopVarName;
        deallocNode.m_symType = SymbolType::INTEGER;
        
        loopNode->addChildNode(deallocNode);
        loopNode->addCommentNode("FOR loop end");

        return ParseResult::valid(loopNode);
    }

    return ParseResult::invalid();
}

Parser::ParseResult Parser::varStatement()
{
    if (match(TOK_VAR))
    {
        if (!match(Lexer::TokenType::IDENT))
        {
            error("Expected an identifier name after VAR");
            return ParseResult::invalid();
        }

        auto varDeclNode = std::make_shared<ASTNode>(ASTNode::NodeType::VARDECL);
        varDeclNode->m_varName = m_matchedToken.m_tokstr;
        varDeclNode->m_symType = SymbolType::INTEGER;

        while(match(Lexer::TokenType::COMMA))
        {
            if (!match(Lexer::TokenType::IDENT))
            {
                error("Expected an identifier name after comma");
                return ParseResult::invalid();
            }

            auto childNode = std::make_shared<ASTNode>(ASTNode::NodeType::VARDECL);
            childNode->m_varName = m_matchedToken.m_tokstr;
            childNode->m_symType = SymbolType::INTEGER;

            varDeclNode->addChildNode(childNode);
        }

        return ParseResult{varDeclNode, true};
    }
    
    return ParseResult::invalid();    
}

Parser::ParseResult Parser::ifStatement()
{
    if (match(TOK_IF))
    {
        auto ifNode = std::make_shared<ASTNode>(ASTNode::NodeType::STATEMENTS);

        auto logic = logicalExpression();
        if (!logic.m_ok)
        {
            error("Expected a logical expression following IF");
            return ParseResult::invalid();
        }

        if (!match(TOK_THEN))
        {
            error("Expected THEN");
            return ParseResult::invalid();
        }

        auto block1 = statements();

        if (!block1.m_ok)
        {
            error("Expected a statement block after THEN");
            return ParseResult::invalid();
        }

        // optional ELSE block
        std::shared_ptr<ASTNode> block2Node = nullptr;
        if (match(TOK_ELSE))
        {
            auto block2 = statements();
            if (!block2.m_ok)
            {
                error("Expected a statement block after ELSE");
                return ParseResult::invalid();
            }
            block2Node = block2.m_node;
        }

        // required ENDIF
        if (!match(TOK_ENDIF))
        {
            error("IF without ENDIF");
            return ParseResult::invalid();
        }

        ifNode->addCommentNode("IF start");
        // first, evaluate the logic expressions and sub
        ifNode->addChildNode(logic.m_node);

        // create a jump over block1 if not equal
        auto jmpLabelId  = m_labelCount++;
        ASTNode jneNode(ASTNode::NodeType::JNE);
        jneNode.m_intValue = jmpLabelId;

        ifNode->addChildNode(jneNode);

        // add block1 statements
        ifNode->addCommentNode("IF block1");
        ifNode->addChildNode(block1.m_node);

        // add block2 statements if it exists
        if (block2Node != nullptr)
        {
            // insert jump over block1 to exit
            auto exitLabelId = m_labelCount++;
            ASTNode jmpNode(ASTNode::NodeType::JMP);
            jmpNode.m_intValue = exitLabelId;

            ifNode->addChildNode(jmpNode);

            ifNode->addLabelNode(jmpLabelId);   // location of 2nd block
            ifNode->addCommentNode("IF block2");
            ifNode->addChildNode(block2Node);   // add block 2 statements

            ifNode->addLabelNode(exitLabelId);
        }
        else
        {
            // only one block
            ifNode->addLabelNode(jmpLabelId);
        }
        ifNode->addCommentNode("IF end");

        return ParseResult::valid(ifNode);
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

Parser::ParseResult Parser::logicalExpression()
{
    auto expr1 = expression();
    if (!expr1.m_ok)
    {
        return ParseResult::invalid();
    }

    char c = 0;
    if (match(Lexer::TokenType::EQUALS))
    {
        c = m_matchedToken.m_tokstr[0];
    }
    else if (match(Lexer::TokenType::SMALLER))
    {
        c = m_matchedToken.m_tokstr[0];
        error("Not supported");
        return ParseResult::invalid();
    }
    else if (match(Lexer::TokenType::GREATER))
    {
        c = m_matchedToken.m_tokstr[0];
        error("Not supported");
        return ParseResult::invalid();
    }
    else if (match(Lexer::TokenType::SMALLEROREQUAL))
    {
        c = m_matchedToken.m_tokstr[0];
        error("Not supported");
        return ParseResult::invalid();
    }
    else if (match(Lexer::TokenType::GREATEROREQUAL))
    {
        c = m_matchedToken.m_tokstr[0];
        error("Not supported");
        return ParseResult::invalid();
    }
    else if (match(Lexer::TokenType::UNEQUAL))
    {
        c = m_matchedToken.m_tokstr[0];
        error("Not supported");
        return ParseResult::invalid();        
    }
    else
    {
        error("Expected a comparison operator");
        return ParseResult::invalid();
    }

    auto expr2 = expression();
    if (!expr2.m_ok)
    {
        return ParseResult::invalid();
    }

    // create SUB node for comparison
    auto subNode = std::make_shared<ASTNode>(ASTNode::NodeType::SUB);
    subNode->addChildNode(expr1.m_node);
    subNode->addChildNode(expr2.m_node);

    return ParseResult::valid(subNode);
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
        auto varNode = std::make_shared<ASTNode>(ASTNode::NodeType::VAR);
        varNode->m_varName = m_matchedToken.m_tokstr;
        varNode->m_pos = m_matchedToken.m_pos;
        return ParseResult::valid(varNode);
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
        numNode->m_pos = m_matchedToken.m_pos;

        unaryNode->addChildNode(numNode);

        return ParseResult::valid(unaryNode);
    }

    error("Invalid factor");
    return ParseResult::invalid();
}

void Parser::error(const char *txt)
{
    std::cout << "Error: " << txt << "  line: " << m_token.m_pos.m_line << " col:" << m_token.m_pos.m_col << "\n";
}
