#!/bin/bash
export PATH=$MXE/usr/bin
FLAGS="-std=gnu++11 -I ../libnpa/include/ -L ../libnpa/ -lnpa -mwindows -lboost_filesystem-mt"
i686-pc-mingw32-g++ src/npaextract.cpp -o npaextract.exe $FLAGS
i686-pc-mingw32-g++ src/npapack.cpp -o npapack.exe $FLAGS
i686-pc-mingw32-g++ src/nsbparse.cpp -o nsbparse.exe $FLAGS
i686-pc-mingw32-g++ src/nsbcompile.cpp -o nsbcompile.exe $FLAGS
i686-pc-mingw32-g++ src/npinstall.cpp -o npinstall.exe $FLAGS
