/* 
 * npapack: .npa file creation utility
 * Copyright (C) 2013-2016,2018 Mislav Blažević <krofnica996@gmail.com>
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
#include "onpafile.hpp"
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

void RecursiveAppend(ONpaFile& Archive, const path& dir)
{
    if (!exists(dir) || !is_directory(dir))
        return;

    directory_iterator end;
    for (directory_iterator i(dir); i != end; ++i)
    {
        if (is_directory(i->path()))
            RecursiveAppend(Archive, i->path());
        else
        {
            cout << "Appending file: " << i->path().string() << "..." << endl;
            Archive.WriteFile(i->path().string());
        }
    }
}

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3)
    {
        cout << "usage: " << argv[0] << " <directory> [charset]" << endl;
        return 1;
    }

    if (argc == 3)
        NpaFile::SetLocale(argv[2]);
    else
        NpaFile::SetLocale("ja_JP.CP932");

    string DirName(argv[1]);
    if (DirName.back() == '/')
        DirName.resize(DirName.size() - 1);

    ONpaFile Archive(DirName + ".npa");
    RecursiveAppend(Archive, path(DirName));
}
