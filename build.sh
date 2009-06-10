#!/bin/bash
PROCESSORS=`grep processor /proc/cpuinfo | wc -l`
declare -i JFLAG
JFLAG=$PROCESSORS+1

if [ -d build ]; then
    cd build
    cmake ../ -DCMAKE_INSTALL_PREFIX=.
    make -j$JFLAG
    make install
else
    mkdir build
    cd build
    cmake ../ -DCMAKE_INSTALL_PREFIX=. && make -j$JFLAG && make install
fi
