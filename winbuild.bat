mkdir build
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=. -G "MinGW Makefiles"
make
make install
