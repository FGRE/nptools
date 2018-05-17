/* 
 * npaextract: .npf file parsing tool
 * Copyright (C) 2018 Mislav Blažević <krofnica996@gmail.com>
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
#include "buffer.hpp"
using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        cout << "usage: " << argv[0] << " <file.npf>" << endl;
        return 1;
    }
    NpaFile::SetLocale("ja_JP.CP932");

    uint32_t Size;
    char* pData = fs::ReadFile(argv[1], Size);
    Npa::Buffer SaveData(NpaFile::Decrypt(pData, Size), Size);

    uint32_t NumVars = SaveData.Read<uint32_t>();
    for (uint32_t i = 0; i < NumVars; ++i)
    {
        string Name1 = NpaFile::ToUtf8(SaveData.ReadStr32());
        string Name2 = NpaFile::ToUtf8(SaveData.ReadStr32());
        uint32_t Type = SaveData.Read<uint32_t>();
        int32_t IntVal = SaveData.Read<int32_t>();
        float FloatVal = SaveData.Read<float>();
        string StrVal = NpaFile::ToUtf8(SaveData.ReadStr32());
        bool unk2 = SaveData.Read<bool>();
        string ArrayRef = NpaFile::ToUtf8(SaveData.ReadStr32());
        cout << "ENTRY " << i << ": " << Name1 << " " << Name2 << " " << Type << " " << IntVal << " " << FloatVal << " " << StrVal << " " << unk2 << " " << ArrayRef << '\n';
    }

    uint32_t NumArrs = SaveData.Read<uint32_t>();
    for (uint32_t i = 0; i < NumArrs; ++i)
    {
        string Name1 = NpaFile::ToUtf8(SaveData.ReadStr32());
        uint32_t NumElems = SaveData.Read<uint32_t>();
        cout << "ENTRY " << i << ": " << Name1 << " " << NumElems << " ";
        for (uint32_t j = 0; j < NumElems; ++j)
        {
            string Value = NpaFile::ToUtf8(SaveData.ReadStr32());
            cout << Value << " ";
        }
        cout << '\n';
    }
}
