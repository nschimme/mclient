#!/bin/bash
set -e
declare -i JFLAG
if [ -e /proc/cpuinfo ]; then
    # Linux
    PROCESSORS=`grep '^processor\s*:' /proc/cpuinfo | wc -l`
    JFLAG=$PROCESSORS+1
elif [ -e /usr/sbin/system_profiler ]; then
    # Mac OS X
    PROCESSORS=`system_profiler SPHardwareDataType -detailLevel mini | grep "Total Number Of Cores:" | cut -d : -f 2`
    JFLAG=$PROCESSORS+1
else
    JFLAG=2
fi

FLAGS=""
MAKE="make -j$JFLAG && make install"

if [[ "`uname -s`" =~ [CYGWIN] ]]; then
    # Cygwin
    GENERATOR="-G \"MinGW Makefiles\""
    FLAGS="$FLAGS -DZLIB_INCLUDE_DIR=$ZLIB_INCLUDE_DIR -DZLIB_LIBRARY=$ZLIB_LIBRARY"
    MAKE="mingw32-make --jobs=$JFLAG  &&  mingw32-make install"
fi

[ -d build ] && rm -r build

mkdir -p build && cd build || exit 1

CMAKE="cmake $GENERATOR ../ -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=. \
-DMCLIENT_BIN_DIR=. -DMCLIENT_PLUGINS_DIR=plugins $FLAGS"
echo $CMAKE
#`$CMAKE`

echo $MAKE
#`echo $MAKE`

ln -sf ../config config
