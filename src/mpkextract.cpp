/* 
 * mpkextract: .mpk file extraction utility
 * Copyright (C) 2013-2016,2018 Mislav Blažević <krofnica996@gmail.com>
 * Copyright (C) 2021 Ivan Pavluk <chayleaf@protonmail.com>
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
#include "impkfile.hpp"
#include "inpafile.hpp"
#include "isgfile.hpp"
#include "fscommon.hpp"
using namespace std;

int PrintArguments(char** argv)
{
    cout << "Supported games:\nSteinsGate\n";
    cout << "usage: " << argv[0] << " <file.mpk> <game> [charset]" << endl;
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
    if (strcmp(argv[2], "SteinsGate") == 0)
        pArchive = new IMpkFile(argv[1]);
    else
        return PrintArguments(argv);

    for (IMpkFile::NpaIterator iter = pArchive->Begin(); iter != pArchive->End(); ++iter)
    {
        if (pArchive->IsDirectory(iter))
            continue;

        char* pData = pArchive->ReadFile(iter);
        cout << "Writing file: " << iter->first << endl;
        fs::WriteFileDirectory(iter->first, pData, pArchive->GetFileSize(iter));
        delete[] pData;
    }
    delete pArchive;
}
