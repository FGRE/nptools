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
#include "fscommon.hpp"

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        cout << "usage: " << argv[0] << " <input> <output>" << endl;
        return 1;
    }

    uint32_t Size;
    char* pData = fs::ReadFile(argv[1] , Size);
    fs::WriteFile(argv[2], NpaFile::Decrypt(pData, Size), Size);
    delete[] pData;
}
