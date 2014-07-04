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
#include "inipafile.hpp"
#include "isgfile.hpp"
#include "fscommon.hpp"
#include <iostream>

const char* NipaGames[] =
{
    "ChaosHead",
    "ChaosHeadTR1",
    "ChaosHeadTR2",
    "MuramasaTR",
    "Muramasa",
    "Sumaga",
    "Django",
    "DjangoTR",
    "Lamento",
    "LamentoTR",
    "SweetPool",
    "SumagaSP",
    "Demonbane",
    "MuramasaAD",
    "Axanel",
    "Kikokugai",
    "SoniComiTR2",
    "Sumaga3P",
    "SoniComi",
    "LostX",
    "LostXTrailer",
    "DramaticalMurder",
    nullptr
};

uint8_t GameStringToID(std::string String)
{
    for (int i = 0; NipaGames[i]; ++i)
        if (String == NipaGames[i])
            return i;
    return -1;
}

int PrintArguments(char** argv)
{
    std::cout << "Supported games:\nSteinsGate\n";
    for (int i = 0; NipaGames[i]; ++i)
        std::cout << NipaGames[i] << '\n';
    std::cout << "usage: " << argv[0] << " <file.npa> <game> [charset]" << std::endl;
    return 1;
}

int main(int argc, char** argv)
{
    if (argc < 3 || argc > 4)
        return PrintArguments(argv);

    if (argc == 4)
        NpaFile::SetLocale(argv[3]);
    else
        NpaFile::SetLocale("ja_JP.CP932");

    INpaFile* pArchive;
    if (std::strcmp(argv[2], "SteinsGate") == 0)
        pArchive = new ISGFile(argv[1]);
    else
    {
        uint8_t GameID = GameStringToID(argv[2]);
        if (GameID == -1)
            return PrintArguments(argv);
        pArchive = new INipaFile(argv[1], GameID);
    }

    for (INipaFile::NpaIterator iter = pArchive->Begin(); iter != pArchive->End(); ++iter)
    {
        // TODO: if IsNotDirectory
        char* pData = pArchive->ReadFile(iter);
        std::cout << "Writing file: " << iter->first << std::endl;
        fs::WriteFileDirectory(iter->first, pData, pArchive->GetFileSize(iter));
        delete[] pData;
    }
}
