mkdir winbuild
cd winbuild
cmake ../ -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=. -G "MinGW Makefiles"
mingw32-make && mingw32-make install
cd ..
