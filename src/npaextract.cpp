/* 
 * npaextract: .npa file extraction utility
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
#include "inpafile.hpp"
#include "fscommon.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3)
    {
        std::cout << "usage: " << argv[0] << " <file.npa> [charset]" << std::endl;
        return 1;
    }

    if (argc == 3)
        NpaFile::SetLocale(argv[2]);
    else
        NpaFile::SetLocale("ja_JP.CP932");

    INpaFile Archive(argv[1]);
    for (INpaFile::NpaIterator iter = Archive.Begin(); iter != Archive.End(); ++iter)
    {
        char* pData = Archive.ReadFile(iter);
        std::cout << "Writing file: " << iter->first << std::endl;
        fs::WriteFileDirectory(iter->first, pData, iter->second.Size);
        delete[] pData;
    }
}
