#!/bin/bash
declare -i JFLAG
OSTYPE=`set | grep OSTYPE | cut -d = -f 1`
if [ "$OSTYPE" == "linux-gnu" ]; then
    PROCESSORS=`grep processor /proc/cpuinfo | wc -l`
    JFLAG=$PROCESSORS+1
else
    JFLAG=2
fi

if [ -d build ]; then
    rm -r build
fi

mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=. \
-DMCLIENT_BIN_DIR=. -DMCLIENT_PLUGINS_DIR=plugins && make -j$JFLAG && make install
ln -sf ../config config
