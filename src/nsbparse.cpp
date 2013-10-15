#include "nsbfile.hpp"

#include <iostream>
#include <fstream>
#include <cstring>
int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "usage: " << argv[0] << " <input.nsb> <output.nss>" << std::endl;
        return 1;
    }

    NsbFile Script(argv[1], NSB_COMPILED);
    std::ofstream File(argv[2]);
    while (Line* pLine = Script.GetNextLine())
    {
        std::cout << "Writing magic: " << std::hex << SWAP_UINT16(pLine->Magic) << std::endl;
        uint32_t i = 0;

        // Cause pLine->Magic == MAGIC_CALL is bugged...
        if (strcmp("CALL", NsbFile::StringifyMagic(pLine->Magic)) == 0 ||
            strcmp("BEGIN", NsbFile::StringifyMagic(pLine->Magic)) == 0)
            File << pLine->Params[i++];
        else
            File << NsbFile::StringifyMagic(pLine->Magic);

        File << "(";
        for (; i < pLine->Params.size(); ++i)
            File << pLine->Params[i] << ((i != (pLine->Params.size() - 1)) ? ", " : "");
        File << ");\n";
        File << std::flush;
    }
}
