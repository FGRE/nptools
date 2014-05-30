/* 
 * npcrypt: Nitroplus game files encrypt/decrypt tool
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
#include "npafile.hpp"
#include <fstream>

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "usage: " << argv[0] << " <input> <output>" << std::endl;
        return 1;
    }

    // Read
    std::ifstream In(argv[1], std::ios::binary);
    In.seekg(0, std::ios::end);
    int Size = In.tellg();
    char* pData = new char[Size];
    In.seekg(0, std::ios::beg);
    In.read(pData, Size);

    // Write
    std::ofstream Out(argv[2], std::ios::binary);
    Out.write(NpaFile::Decrypt(pData, Size), Size);
    delete[] pData;
}
