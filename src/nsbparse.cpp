#include "nsbfile.hpp"
#include "nsbmagic.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "usage: " << argv[0] << " <input.nsb> <output.nss>" << std::endl;
        return 1;
    }

    std::vector<uint16_t> UnkMagic;
    NsbFile Script(argv[1]);
    std::ofstream File(argv[2]);
    uint16_t unk = 162;

    while (Line* pLine = Script.GetNextLine())
    {
        uint32_t i = 0;

        if (!NsbFile::IsValidMagic(pLine->Magic))
        {
            if (std::find(UnkMagic.begin(), UnkMagic.end(), pLine->Magic) == UnkMagic.end())
            {
                std::cout << std::uppercase << "MAGIC_UNK" << std::dec << unk++ << " = " << "SWAP_UINT16(0x"
                          << std::hex << uint16_t(SWAP_UINT16(pLine->Magic)) << ")," << std::endl;
                UnkMagic.push_back(pLine->Magic);
            }
            continue;
        }

        // Cause pLine->Magic == MAGIC_CALL is bugged...
        if (pLine->Magic == uint16_t(MAGIC_CALL) || pLine->Magic == uint16_t(MAGIC_BEGIN))
            File << pLine->Params[i++];
        else
            File << NsbFile::StringifyMagic(pLine->Magic);

        File << "(";
        for (; i < pLine->Params.size(); ++i)
            File << pLine->Params[i] << ((i != (pLine->Params.size() - 1)) ? ", " : "");
        File << ");\n";
        File << std::flush;
    }

    for (uint32_t i = 0; i < UnkMagic.size(); ++i)
        std::cout << "(" << "MAGIC_UNK" << std::dec << unk - UnkMagic.size() + i << ", "
                  << "\"UNK" << unk - UnkMagic.size() + i << "\")" << std::endl;
}
