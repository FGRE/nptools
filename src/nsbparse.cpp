/* 
 * nsbparse: .nsb script decompiler
 * Copyright (C) 2013-2014 Mislav Blažević <krofnica996@gmail.com>
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
#include "scriptfile.hpp"
#include "nsbmagic.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cstring>

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3)
    {
        std::cout << "usage: " << argv[0] << "[-l] <input.nsb>" << std::endl;
        return 1;
    }

    bool PrintLineNumbers = false;
    if (argc == 2)
    {
        if (argv[1][0] == '-')
        {
            std::cout << "No input file specified" << std::endl;
            return 1;
        }
    }
    else
    {
        if (std::string(argv[1]) != "-l")
        {
            std::cout << "Invalid option specified" << std::endl;
            return 1;
        }
        else
            PrintLineNumbers = true;
    }

    std::string Output = argv[PrintLineNumbers ? 2 : 1];
    Output[Output.size() - 1] = 's';
    Output = Output.substr(Output.find_last_of("/") + 1);

    ScriptFile Script(argv[PrintLineNumbers ? 2 : 1]);
    std::ofstream File(Output);
    int indent = -1;
    while (Line* pLine = Script.GetNextLine())
    {
        uint32_t i = 0;

        if (PrintLineNumbers)
            File << std::setfill('0') << std::setw(5) << Script.GetNextLineEntry() - 1 << " ";

        if (!Nsb::IsValidMagic(pLine->Magic))
        {
            std::cout << "Unknown magic: " << std::hex << pLine->Magic << std::dec << std::endl;
            continue;
        }

        if (pLine->Magic == MAGIC_SCOPE_BEGIN)
            ++indent;
        for (int i = 0; i < indent; ++i)
            File << "    ";
        if (pLine->Magic == MAGIC_SCOPE_END)
            --indent;

        if (pLine->Magic == MAGIC_CALL_FUNCTION || pLine->Magic == MAGIC_FUNCTION_BEGIN)
            File << pLine->Params[i++];
        else
            File << Nsb::StringifyMagic(pLine->Magic);

        File << "(";
        for (; i < pLine->Params.size(); ++i)
            File << pLine->Params[i] << ((i != (pLine->Params.size() - 1)) ? ", " : "");
        File << ");\n";
    }
}
