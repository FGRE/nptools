/* 
 * nsbcompile: .nsb script compiler
 * Copyright (C) 2013 Mislav Blažević <krofnica996@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * */
#include "nsbfile.hpp"
#include "nsbmagic.hpp"

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

    while (getline(Script, SLine))
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
        else if (SLine.find("function.") == 0 || SLine.find("macrosys") == 0)
        {
            uint16_t NsbBegin = uint16_t(MAGIC_BEGIN);
            Binary.write((char*)&NsbBegin, sizeof(uint16_t));
            Binary.write((char*)&NumParams, sizeof(uint16_t));
            Binary.write((char*)&it, sizeof(uint32_t));
            Binary.write(SLine.c_str(), it);
        }
        else
        {
            uint16_t NsbCall = uint16_t(MAGIC_CALL);
            Binary.write((char*)&NsbCall, sizeof(uint16_t));
            Binary.write((char*)&NumParams, sizeof(uint16_t));
            Binary.write((char*)&it, sizeof(uint32_t));
            Binary.write(SLine.c_str(), it);
        }
        SLine[it] = '(';

        Binary.write((char*)&NumParams, sizeof(uint16_t));

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
