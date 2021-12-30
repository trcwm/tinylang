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

    if (!parser.parse())
    {
        std::cout << "MEH!\n";
        return EXIT_FAILURE;
    }
    else
    {
        std::cout << "YAY!\n";
    }

    return EXIT_SUCCESS;
}

