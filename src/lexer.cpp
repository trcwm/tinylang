#include <algorithm>
#include "lexer.h"

bool Lexer::Lex::registerKeyword(const std::string &kw, uint16_t id)
{
    if (id >= static_cast<uint16_t>(TokenType::KEYWORD))
    {
        m_keywords[kw] = id;
        return true;
    }
    
    return false;
}

char Lexer::Lex::advance()
{
    m_curpos.m_col++;
    m_is.next();
    return peek();
}

void Lexer::Lex::enableCaseSensitivity()
{
    m_caseSensitive = true;
}

void Lexer::Lex::disableCaseSensitivity()
{
    m_caseSensitive = false;
}

void Lexer::Lex::addExtendedAlphaChar(char c)
{
    m_extendedAlphaCharacters += c;
}

Lexer::Token Lexer::Lex::nextToken()
{
    m_curtok.m_tokstr.clear();
    m_curtok.m_type = TokenType::UNKNOWN;
    m_curtok.m_integer = 0;

    LexState state = LexState::IDLE;
    char c = peek();

    while(c != 0)
    {
        c = peek();

        // if we have an EOF, return
        // token first, unless there isn't one
        // then return an EOF token
        if (c == 0)
        {
            if (m_curtok.m_type  == TokenType::UNKNOWN)
            {
                m_curtok.m_type = TokenType::ENDFILE;
            }
            return m_curtok;
        }

        switch(state)
        {
        case LexState::IDLE:            

            while (isWhitespace(c))
            {
                c = advance();
            }
            
            if (isEOL(c))
            {
                // increment to the next line
                m_curpos.nextLine();

                auto nextChar = advance();

                if (isEOL(nextChar) && (nextChar != c))
                {
                    // this is a 10,13 or 13,10 sequence
                    // so we should also eat the additional LF or CR.
                    advance();
                }
            }
            else if (c == ',')
            {
                m_curtok.m_type = TokenType::COMMA;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                advance();
                return m_curtok;
            }            
            else if (c == '=')
            {
                m_curtok.m_type = TokenType::EQUALS;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                advance();
                return m_curtok;
            }
            else if (c == '+')
            {
                m_curtok.m_type = TokenType::PLUS;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                advance();
                return m_curtok;
            }            
            else if (c == '-')
            {
                m_curtok.m_type = TokenType::MINUS;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                advance();
                return m_curtok;
            }            
            else if (c == '*')
            {
                m_curtok.m_type = TokenType::STAR;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                advance();
                return m_curtok;
            }
            else if (c == '/')
            {
                m_curtok.m_type = TokenType::SLASH;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                advance();
                return m_curtok;
            }            
            else if (c == '(')
            {
                m_curtok.m_type = TokenType::LPAREN;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                advance();
                return m_curtok;
            }            
            else if (c == ')')
            {
                m_curtok.m_type = TokenType::RPAREN;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                advance();
                return m_curtok;
            }
            else if (c == '[')
            {
                m_curtok.m_type = TokenType::LBRACKET;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                advance();
                return m_curtok;
            }            
            else if (c == ']')
            {
                m_curtok.m_type = TokenType::RBRACKET;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                advance();
                return m_curtok;
            }            
            else if (c == '$')
            {
                m_curtok.m_type = TokenType::DOLLAR;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                advance();
                return m_curtok;
            }            
            else if (c == '>')
            {
                m_curtok.m_type = TokenType::GREATER;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                c = advance();

                if (c == '=')
                {
                    m_curtok.m_tokstr += c;
                    advance();
                    m_curtok.m_type = TokenType::GREATEROREQUAL;
                }

                return m_curtok;
            }            
            else if (c == '<')
            {
                m_curtok.m_type = TokenType::SMALLER;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                advance();

                if (c == '=')
                {
                    m_curtok.m_tokstr += c;
                    advance();
                    m_curtok.m_type = TokenType::SMALLEROREQUAL;
                }

                return m_curtok;
            }
            else if (c == ':')
            {
                m_curtok.m_type = TokenType::COLON;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                advance();
                return m_curtok;
            }            
            else if (c == ';')
            {
                m_curtok.m_type = TokenType::SEMICOL;
                m_curtok.m_tokstr = c;
                m_curtok.m_pos = m_curpos;
                advance();
                return m_curtok;
            }
            else if (c == '"')
            {
                // ignore the starting quote
                m_curtok.m_type = TokenType::STRING;
                m_curtok.m_pos = m_curpos;
                state = LexState::STRING;                
                advance();
            }
            else if (isDigit(c))
            {
                state = LexState::NUMBER;
                m_curtok.m_integer = c - '0';
                m_curtok.m_tokstr += c;
                m_curtok.m_type = TokenType::NUMBER;
                m_curtok.m_pos = m_curpos;
                advance();
            }
            else if (isExtendedAlpha(c))
            {
                state = LexState::IDENT;
                m_curtok.m_tokstr += c;
                m_curtok.m_type = TokenType::IDENT;
                m_curtok.m_pos = m_curpos;
                advance();
            }
            else
            {
                // lexer fail!
                m_curtok.m_type = TokenType::ERROR;
                m_curtok.m_pos = m_curpos;
                return m_curtok;
            }
            break;
        case LexState::NUMBER:
            if (isDigit(c))
            {
                m_curtok.m_integer *= 10;
                m_curtok.m_integer += c - '0';
                m_curtok.m_tokstr += c;
                advance();
            }
            else
            {
                state = LexState::IDLE;
                return m_curtok;
            }
            break;
        case LexState::IDENT:
            if (isExtendedAlphaOrDigit(c))
            {
                m_curtok.m_tokstr += c;
                advance();
            }
            else
            {
                state = LexState::IDLE;

                updateTokenIfKeyword();

                return m_curtok;            
            }
            break;
        case LexState::STRING:
            // FIXME: what about escape characters?
            if (c != '"')
            {
                m_curtok.m_tokstr += c;
                advance();
            }
            else
            {
                // no need for unget:
                // the end quote should be ignored.
                state = LexState::IDLE;
                advance();
                return m_curtok;
            }
            break;
        }
    }

    if (m_is.atEnd())
    {
        m_curtok.m_type = TokenType::ENDFILE;
    }

    return m_curtok;
}

void Lexer::Lex::updateTokenIfKeyword()
{
    auto ident = m_curtok.m_tokstr;

    if (!m_caseSensitive)
    {
        std::for_each(ident.begin(), ident.end(), [](char & c)
        {
            c = ::tolower(c);
        });
    }

    auto iter = m_keywords.find(ident);
    if (iter != m_keywords.end())
    {
        m_curtok.m_keyword = iter->second;
    }
}
