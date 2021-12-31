#include <iostream>
#include <fstream>
#include <stdint.h>

#include "parser.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input file>\n";
        return EXIT_FAILURE;
    }

    std::ifstream ifile(argv[1]);

    FileSrcStream fstream(ifile);

    if (!ifile.is_open())
    {
        std::cerr << "Could not open file " << argv[1] << "\n";
        return EXIT_FAILURE;
    }

    Parser parser(fstream);

    auto parseResults = parser.parse();

    if (!parseResults.m_ok)
    {
        std::cout << "MEH!\n";
        return EXIT_FAILURE;
    }
    else
    {
        std::cout << "YAY!\n";

        // dump ast
        std::cout << "Dumping AST:\n\n";
        parseResults.m_node->dump(std::cout, 0);
    }




    return EXIT_SUCCESS;
}

