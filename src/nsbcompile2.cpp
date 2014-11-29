/* 
 * nsbcompile2: Nitroplus script compiler
 * Copyright (C) 2014 Mislav Blažević <krofnica996@gmail.com>
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
#include <iostream>
#include <fstream>
#include <cstring>
#include "scriptfile.hpp"
#include "npafile.hpp"
#include "fscommon.hpp"
#include "buffer.hpp"
using namespace std;

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3)
    {
        std::cout << "usage: " << argv[0] << " <input.nss> [charset]" << std::endl;
        return 1;
    }

    if (argc == 3)
        NpaFile::SetLocale(argv[2]);
    else
        NpaFile::SetLocale("ja_JP.CP932");

    ofstream Output(string(argv[1], 0, strlen(argv[1]) - 3) + "nsb", ios::binary);
    ofstream MapOutput(string(argv[1], 0, strlen(argv[1]) - 3) + "map", ios::binary);

    NpaPrivate::Buffer NsbBuffer, MapBuffer;
    uint32_t Size;
    char* pBuffer = fs::ReadFile(argv[1], Size);
    Nss::Compile(pBuffer, Size, &NsbBuffer, &MapBuffer);
    delete[] pBuffer;

    Output.write(&NsbBuffer.Data[0], NsbBuffer.Size());
    MapOutput.write(&MapBuffer.Data[0], MapBuffer.Size());
}
