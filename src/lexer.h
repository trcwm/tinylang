#pragma once

#include <cstdio>
#include <cstdint>
#include <string>
#include <iostream>
#include <unordered_map>

#include "mytypes.h"
#include "inputstream.h"

namespace Lexer
{

    enum class TokenType : uint16_t
    {
        UNKNOWN = 0,
        ENDFILE,
        ERROR,
        IDENT,
        NUMBER,
        STRING,
        COMMA,
        EQUALS,
        PLUS,
        MINUS,
        STAR,
        SLASH,
        COLON,      // :
        SEMICOL,    // ;
        LPAREN,     // (
        RPAREN,     // )
        LBRACKET,   // [
        RBRACKET,   // ]        
        DOLLAR,     // $
        GREATER,    // >
        SMALLER,    // <
        GREATEROREQUAL, // >=
        SMALLEROREQUAL, // <=
        UNEQUAL,        // !=
        KEYWORD = 1000
    };

    struct Token
    {
        union
        {
            TokenType   m_type;
            uint16_t     m_keyword;
        };

        int16_t     m_integer;
        std::string m_tokstr;

        LinePos     m_pos;
    };

    enum class LexState
    {
        IDLE = 0,
        IDENT,
        NUMBER,
        STRING
    };

    class Lex
    {
    public:
        Lex(ISrcStream &is) : 
            m_is(is), 
            m_caseSensitive(true),
            m_curpos{1,0} 
        {            
        }

        Token getCurrentToken() const noexcept
        {
            return m_curtok;
        }

        Token nextToken();
        
        /** Register a keyword. The string should be lower case if
         *  case insensitivity is enabled.
         *  id should be >= 1000
        */
        bool registerKeyword(const std::string &kw, uint16_t id);

        /** Add a character to the extended alpha characters list.
         *  
        */
        void addExtendedAlphaChar(char c);

        std::string getExtendedAlphaChars() const
        {
            return m_extendedAlphaCharacters;
        }

        void enableCaseSensitivity();
        void disableCaseSensitivity();

    protected:
        bool isEOL(char c)
        {
            return (c == '\n') || (c == '\r');
        }

        bool isWhitespace(char c)
        {
            return (c == ' ') || (c == '\t');
        }

        bool isDigit(char c)
        {
            return (c >= '0') && (c <= '9');
        }

        bool isAlpha(char c)
        {
            return ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'));
        }

        bool isExtendedAlpha(char c)
        {
            if (m_extendedAlphaCharacters.find(c) != std::string::npos)
            {
                return true;
            }

            return isAlpha(c);
        }

        bool isExtendedAlphaOrDigit(char c)
        {
            return isExtendedAlpha(c) || isDigit(c);
        }

        void updateTokenIfKeyword();
        char advance();
        
        char peek() const noexcept
        {
            return m_is.peek();
        }

        ISrcStream &m_is;
        Token m_curtok;
        LinePos m_curpos;

        std::unordered_map<std::string, uint16_t> m_keywords;
        std::string m_extendedAlphaCharacters;

        bool m_caseSensitive;
    };

};
