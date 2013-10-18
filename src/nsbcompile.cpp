#include "nsbfile.hpp"

#include <iostream>
#include <cstring>
#include <fstream>

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "usage: " << argv[0] << " <input.nss> <output.nsb>" << std::endl;
        return 1;
    }

    std::ifstream Script(argv[1]);
    std::ofstream Binary(argv[2], std::ios::binary);
    std::string SLine;
    uint32_t Entry = 1;

    while (getline(Script, SLine, ';'))
    {
        if (uint16_t Magic = NsbFile::MagicifyString(SLine.c_str()))
        {
            Binary.write((char*)&Entry++, sizeof(uint32_t));
            Binary.write((char*)&Magic, sizeof(uint16_t));
            // TODO: Num params
        }
    }
}
