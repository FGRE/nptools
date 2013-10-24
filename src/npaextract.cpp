#include "npafile.hpp"

#include <iostream>
#include <boost/locale.hpp>
#include <boost/filesystem.hpp>
using namespace boost::locale;

void WriteFile(char* Data, uint32_t Size, std::string FileName)
{
    // Create directories
    std::string Path = FileName;
    char* delim = strchr((char*)FileName.c_str(), '/');
    do
    {
        *delim = 0;
        if (!boost::filesystem::exists(boost::filesystem::path(Path)))
            boost::filesystem::create_directory(boost::filesystem::path(Path));
        *delim = '/';
    } while ((delim = strchr(delim + 1, '/')));

    // Create file
    std::ofstream Out(FileName, std::ios::binary | std::ios::out);
    Out.write(Data, Size);
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << " <file.npa>" << std::endl;
        return 1;
    }

    std::locale loc = generator().generate("ja_JP.SHIFT-JIS");

    NpaFile Achieve(argv[1], NPA_READ);
    for (NpaIterator File = Achieve.Begin(); File != Achieve.End(); ++File)
    {
        std::string FileName = conv::to_utf<char>(File.GetFileNameRaw(), File.GetFileNameRaw() + File.GetFileNameSize(), loc);
        std::cout << "Writing file: " << FileName << "..." << std::endl;
        WriteFile(File.GetFileData(), File.GetFileSize(), FileName);
    }
}
