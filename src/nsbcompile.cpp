#include "nsbfile.hpp"

#include <iostream>
#include <algorithm>
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
    uint16_t NumParams;

    while (getline(Script, SLine, ';') && SLine != "\n")
    {
        SLine.erase(std::remove_if(SLine.begin(), SLine.end(), isspace), SLine.end());
        NumParams = std::count(SLine.begin(), SLine.end(), ',') + 1;
        std::size_t it = SLine.find("("), Delim;

        Binary.write((char*)&Entry, sizeof(uint32_t));

        SLine[it] = 0;
        if (uint16_t Magic = NsbFile::MagicifyString(SLine.c_str()))
        {
            Binary.write((char*)&Magic, sizeof(uint16_t));
            Binary.write((char*)&NumParams, sizeof(uint16_t));
        }
        else
        {
            // TODO: script-defined
        }
        SLine[it] = '(';

        do
        {
            ++it;
            Delim = SLine.find(",", it);
            if (Delim == std::string::npos)
                Delim = SLine.find(")", it);
            if (Delim == std::string::npos)
            {
                std::cout << "Compile error at: " << SLine << std::endl;
                return 2;
            }
            uint32_t Size = Delim - it;
            Binary.write((char*)&Size, sizeof(uint32_t));
            Binary.write(&SLine[it], Size);
            it = Delim;
        } while (SLine[Delim] != ')');

        ++Entry;
    }
}
