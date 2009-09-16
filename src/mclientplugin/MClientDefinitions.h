#ifndef MCLIENTDEFINITIONS_H
#define MCLIENTDEFINITIONS_H

#define bit1 1
#define bit2 2
#define bit3 4
#define bit4 8
#define bit5 16
#define bit6 32
#define bit7 64
#define bit8 128
#define bit9 256
#define bit10 512
#define bit11 1024
#define bit12 2048
#define bit13 4096
#define bit14 8192
#define bit15 16384
#define bit16 32768
#define bit17 65536
#define bit18 131072
#define bit19 262144
#define bit20 524288
#define bit21 1048576
#define bit22 2097152
#define bit23 4194304
#define bit24 8388608
#define bit25 16777216
#define bit26 33554432
#define bit27 67108864
#define bit28 134217728
#define bit29 268435456
#define bit30 536870912
#define bit31 1073741824
#define bit32 2147483648

#define UNSET(dest,bit) (dest &=~bit)
#define SET(dest,bit) (dest |=bit)
#define ISSET(src,bit) (src & bit)
#define ISNOTSET(src,bit) (!(src & bit))
#define KEEP(dest,bit) (dest = dest)

#endif /* MCLIENTDEFINITIONS_H */
