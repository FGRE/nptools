/* 
 * npinstall: Nitroplus game installer
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
#include <libunshield.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include "npafile.hpp"
using namespace boost::filesystem;
using namespace std;

const char* Directories[2] =
{
    "ゲームデータ", // Game data
    "ムービー" // Movies
};

void ExtractFile(Unshield* pUnshield, int index, string OutputDirectory)
{
    string FileName = OutputDirectory + string("/");

    if (!exists(path(OutputDirectory)))
        create_directory(path(OutputDirectory));

    FileName += unshield_file_name(pUnshield, index);
    FileName = NpaFile::ToUtf8(FileName);

    cout << "Extracting: " << FileName << endl;
    if (!unshield_file_save(pUnshield, index, FileName.c_str()))
    {
        cout << "Failed to extract file: " << unshield_file_name(pUnshield, index) << endl;
        unlink(FileName.c_str());
    }
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cout << "usage: " << argv[0] << " <source> <destination>" << endl;
        return 1;
    }

    NpaFile::SetLocale("ja_JP.CP932");
    string MovieDirectory = string(argv[2]) + "/dx";
    string CabPath = string(argv[1]) + "/product/data2.cab";
    unshield_set_log_level(UNSHIELD_LOG_LEVEL_WARNING);
    Unshield* pUnshield = unshield_open(CabPath.c_str());

    if (!pUnshield)
    {
        cout << "Failed to open " << CabPath << endl;
        return 1;
    }

    for (int i = 0; i < unshield_file_group_count(pUnshield); ++i)
    {
        if (UnshieldFileGroup* pFileGroup = unshield_file_group_get(pUnshield, i))
        {
            string Utf8Name = NpaFile::ToUtf8(pFileGroup->name);

            const char* OutputDirectory;
            if (Utf8Name == Directories[1])
                OutputDirectory = MovieDirectory.c_str();
            else if (Utf8Name == Directories[0])
                OutputDirectory = argv[2];
            else continue;

            for (int j = pFileGroup->first_file; j <= pFileGroup->last_file; ++j)
                if (unshield_file_is_valid(pUnshield, j))
                    ExtractFile(pUnshield, j, OutputDirectory);
        }
    }
}
