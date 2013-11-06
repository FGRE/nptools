g++ -o npaextract -Wl,-rpath ../libnpa -ggdb -lboost_locale -lboost_filesystem -lboost_system --std=c++11 -I ../libnpa/include -lnpa -L ../libnpa/ src/npaextract.cpp
g++ -o npapack -Wl,-rpath ../libnpa -ggdb --std=c++11 -I ../libnpa/include -lnpa -lboost_filesystem -lboost_system -L ../libnpa/ src/npapack.cpp
g++ -o nsbparse -Wl,-rpath ../libnpa -ggdb --std=c++11 -I ../libnpa/include -lnpa -L ../libnpa/ src/nsbparse.cpp
g++ -o nsbcompile -Wl,-rpath ../libnpa -ggdb --std=c++11 -I ../libnpa/include -lnpa -L ../libnpa/ src/nsbcompile.cpp -lboost_locale
