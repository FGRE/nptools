#include "npafile.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << " <file.npa>" << std::endl;
        return 1;
    }

    NpaFile Achieve(argv[1], NPA_READ);
    for (NpaIterator File = Achieve.Begin(); File != Achieve.End(); ++File)
    {
        std::cout << "Writing file: " << File.GetFileName() << "..." << std::endl;
        File.Save();
    }
}
