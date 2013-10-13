#include "nsbfile.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << " <file.nsb>" << std::endl;
        return 1;
    }

    NsbFile(argv[1], NSB_COMPILED);
}
