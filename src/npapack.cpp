#include "npafile.hpp"

#include <boost/filesystem.hpp>
#include <iostream>
using namespace boost::filesystem;

void RecursiveAppend(NpaFile& Achieve, const path& dir)
{
    if (!exists(dir) || !is_directory(dir))
        return;

    directory_iterator end;
    for (directory_iterator i(dir); i != end; ++i)
    {
        if (is_directory(i->path()))
            RecursiveAppend(Achieve, dir);
        else
        {
            std::cout << "Appending file: " << i->path().string() << "..." << std::endl;
            Achieve.AppendFile(i->path().string());
        }
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "usage: " << argv[0] << " <directory>" << std::endl;
        return 1;
    }

    NpaFile Achieve(argv[1], NPA_CREATE);
    RecursiveAppend(Achieve, path(argv[1]));
}
