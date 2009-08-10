#!/bin/bash
PROCESSORS=`grep processor /proc/cpuinfo | wc -l`
declare -i JFLAG
JFLAG=$PROCESSORS+1

if [ -d build ]; then
    rm -r build
fi

mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=. && make -j$JFLAG && make install
ln -sf ../config config