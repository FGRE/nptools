/* 
 * nsbparse: .nsb script decompiler
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
#include "mapfile.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3)
    {
        std::cout << "usage: " << argv[0] << " <input.nsb> [output.nss]" << std::endl;
        return 1;
    }

    std::string Output;
    if (argc == 2)
    {
        Output = argv[1];
        Output[Output.size() - 1] = 's';
        Output = Output.substr(Output.find_last_of("/") + 1);
    }
    else
        Output = argv[2];

    std::vector<uint16_t> UnkMagic;
    NsbFile Script(argv[1]);
    std::ofstream File(Output);
    uint16_t unk = MAGIC_UNKMAX;

    while (Line* pLine = Script.GetNextLine())
    {
        uint32_t i = 0;

        if (!NsbFile::IsValidMagic(pLine->Magic))
        {
            if (std::find(UnkMagic.begin(), UnkMagic.end(), pLine->Magic) == UnkMagic.end())
            {
                std::cout << std::uppercase << "MAGIC_UNK" << std::dec << unk++ << " = " << "SWAP_UINT16(0x"
                          << std::hex << uint16_t(SWAP_UINT16(pLine->Magic)) << ")," << std::endl;
                UnkMagic.push_back(pLine->Magic);
            }
            continue;
        }

        if (pLine->Magic == uint16_t(MAGIC_CALL) || pLine->Magic == uint16_t(MAGIC_FUNCTION_BEGIN))
            File << pLine->Params[i++];
        else
            File << NsbFile::StringifyMagic(pLine->Magic);

        File << "(";
        for (; i < pLine->Params.size(); ++i)
            File << pLine->Params[i] << ((i != (pLine->Params.size() - 1)) ? ", " : "");
        File << ");\n";
    }

    for (uint32_t i = 0; i < UnkMagic.size(); ++i)
        std::cout << "(" << "MAGIC_UNK" << std::dec << unk - UnkMagic.size() + i << ", "
                  << "\"UNK" << unk - UnkMagic.size() + i << "\")" << std::endl;

    // Open Map file
    std::strcpy(&argv[1][std::strlen(argv[1]) - 3], "map");
    MapFile MFile(argv[1]);
    std::strcpy(&Output[Output.size() - 3], "map");
    std::ofstream MapOutput(Output);

    while (MapLine* pLine = MFile.GetNextLine())
        MapOutput << pLine->Label << '\n';
}
