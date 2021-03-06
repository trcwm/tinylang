#pragma once

#include <array>
#include "lexer.h"
#include "ast.h"

class Parser
{
public:
    Parser(ISrcStream &is) : m_lex(is)
    {
        setupLexer();
    }

    struct ParseResult
    {
        std::shared_ptr<ASTNode> m_node;
        bool     m_ok;

        static ParseResult invalid()
        {
            return ParseResult{nullptr, false};
        }

        static ParseResult valid(std::shared_ptr<ASTNode> &node)
        {
            return ParseResult{node, true};
        }        
    };

    ParseResult parse();

protected:
    void setupLexer();

    ParseResult statements();
    ParseResult statement();
    ParseResult assignment();
    ParseResult assignment_rhs();
    ParseResult logicalExpression();
    ParseResult expression();
    ParseResult term();
    ParseResult factor();

    ParseResult forStatement();
    ParseResult varStatement();
    ParseResult ifStatement();

    bool match(uint16_t id);
    bool match(Lexer::TokenType type);
    bool checkToken(Lexer::TokenType type);

    void error(const char *txt);

    Lexer::Lex   m_lex;
    Lexer::Token m_token;
    Lexer::Token m_matchedToken;

    int m_labelCount = 0;

    struct KeywordDef
    {
        const char *name;
        uint16_t   id;
    };

    static constexpr uint16_t TOK_FOR = 1000;
    static constexpr uint16_t TOK_TO  = 1001;
    static constexpr uint16_t TOK_ENDFOR = 1002;
    static constexpr uint16_t TOK_VAR = 1003;    
    static constexpr uint16_t TOK_IF  = 1004;
    static constexpr uint16_t TOK_THEN = 1005;
    static constexpr uint16_t TOK_ELSE = 1006;
    static constexpr uint16_t TOK_ENDIF = 1007;

    static constexpr std::array<KeywordDef, 8> ms_keywords = {{
        {"for", TOK_FOR},
        {"to", TOK_TO},
        {"endfor", TOK_ENDFOR},
        {"var", TOK_VAR},
        {"if", TOK_IF},
        {"then", TOK_THEN},
        {"else", TOK_ELSE},
        {"endif", TOK_ENDIF}
    }};
};


