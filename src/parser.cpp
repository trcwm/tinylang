#include "parser.h"

void Parser::setupLexer()
{
    m_lex.addExtendedAlphaChar('_');

    for(const auto &kw : ms_keywords)
    {
        m_lex.registerKeyword(kw.name, kw.id);
    }

    /*
    m_lex.registerKeyword("then", 1001);
    m_lex.registerKeyword("else", 1002);
    m_lex.registerKeyword("endif",1004);

    m_lex.registerKeyword("integer", 1005);
    m_lex.registerKeyword("char", 1006);
    m_lex.registerKeyword("array", 1007);
    m_lex.registerKeyword("function",1008);
    m_lex.registerKeyword("endfunction",1009);
    
    m_lex.registerKeyword("for",  1010);
    m_lex.registerKeyword("to",   1011);
    m_lex.registerKeyword("endfor", 1012);
    
    m_lex.registerKeyword("return",  1013);
    m_lex.registerKeyword("ref",  1014);
    */
}

bool Parser::parse()
{
    m_token = m_lex.nextToken();
    return statements();
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

bool Parser::statements()
{
    while(m_token.m_type != Lexer::TokenType::ENDFILE)
    {
        if (!statement())
        {
            error("Invalid statement");
            return false;
        }
    };

    return true;
}

bool Parser::statement()
{
    if (match(Lexer::TokenType::IDENT))
    {

        return assignment();
    }

    error("Assignment expected");
    return false;
}

bool Parser::assignment()
{
    if (!match(Lexer::TokenType::EQUALS))
    {
        error("Assignment expected '='");
        return false;
    }

    return expression();
}

bool Parser::expression()
{
    if (!term())
    {
        return false;
    }

    while(match(Lexer::TokenType::PLUS) || match(Lexer::TokenType::MINUS))
    {
        auto op = m_matchedToken.m_tokstr;
        if (!term())
        {
            return false;
        }
        std::cout << "OP: " << op << "\n";
    }

    return true;
}

bool Parser::term()
{
    if (!factor())
    {
        return false;
    }

    while(match(Lexer::TokenType::STAR) || match(Lexer::TokenType::SLASH))
    {
        auto op = m_matchedToken.m_tokstr;

        if (!factor())
        {
            return false;
        }

        std::cout << "OP: " << op << "\n";
    }

    return true;
}

bool Parser::factor()
{
    if (match(Lexer::TokenType::LPAREN))
    {
        if (!expression())
        {
            error("Expected an expression");
            return false;
        }
        
        if (!match(Lexer::TokenType::RPAREN))
        {
            error("Expected a closing )");
            return false;
        }
        return true;
    }

    if (match(Lexer::TokenType::NUMBER))
    {
        std::cout << "NUM: " << m_matchedToken.m_tokstr << "\n";
        return true;
    }

    if (match(Lexer::TokenType::IDENT))
    {
        std::cout << "VAR: " << m_matchedToken.m_tokstr << "\n";
        return true;
    }

    if (match(Lexer::TokenType::MINUS))
    {
        if (!match(Lexer::TokenType::NUMBER))
        {
            error("Expected number after unary minus");
            return false;
        }
        std::cout << "- (unary)\n";
        return true;
    }

    error("Invalid factor");
    return false;
}

void Parser::error(const char *txt)
{
    std::cout << "Error: " << txt << "  line: " << m_token.m_pos.m_line << " col:" << m_token.m_pos.m_col << "\n";
}
