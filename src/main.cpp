#include <iostream>
#include <fstream>
#include <stdint.h>

#include "parser.h"

int main(int argc, char *argv[])
{
    std::ifstream ifile(argv[1]);

    FileSrcStream fstream(ifile);

    if (!ifile.is_open())
    {
        std::cerr << "Could not open file " << argv[1] << "\n";
        return EXIT_FAILURE;
    }


    Parser parser(fstream);

    if (!parser.parse())
    {
        std::cout << "MEH!\n";
        return EXIT_FAILURE;
    }

/*
    auto tok = lexer.nextToken();
    while((tok.m_type != Lexer::TokenType::ERROR) &&
        (tok.m_type != Lexer::TokenType::ENDFILE))
    {
        std::cout << "tok: (" << tok.m_pos.m_line << "," << tok.m_pos.m_col << ")  " << tok.m_tokstr << " " << static_cast<int>(tok.m_type) << "\n";
        tok = lexer.nextToken();
    }

    std::cout << "tok: " << static_cast<int>(tok.m_type) << "\n";
    
*/

    return EXIT_SUCCESS;
}

