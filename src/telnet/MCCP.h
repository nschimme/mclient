/***************************************************************************
                          MCCP.h  -  MCCP support
    copyright            : (C) 2002-2008 by Tomas Mecir

    This file has been modified for the mClient distribution from KMuddy.

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef MCCP_H
#define MCCP_H

#include <zlib.h>

class Telnet;

/**
MCCP v1/v2 parser and decompressor.
  *@author Tomas Mecir
  */

class MCCP {
public: 
  MCCP (Telnet *_telnet);
  ~MCCP ();
  /** Prepare decompression, params are input/output buffers and size of
  output buffer. */
  void prepareDecompression (char *_inBuf, char *_outBuf, int inSize, int buflen);
  /** Uncompress as much as fits in the output buffer. Uncompressed
  portions are placed there as well. */
  int uncompressNext ();
  /** MCCP v1 changes state */
  void setMCCP1 (bool val);
  /** MCCP v2 changes state */
  void setMCCP2 (bool val);
  /** returns amount of compressed bytes received so far */
  int compressedBytes () { return compressed; };
  /** returns size of all received data after decompression */
  int uncompressedBytes () { return decompressed; };
  /** reset the object */
  void reset ();
  bool usingMCCP () { return usemccp; };
  int MCCPVer () { return usemccp ? (mccp2 ? 2 : 1) : 0; };
private:
  /** this function does the actual decompression */
  int doUncompressNext ();
  
  char *inBuf;
  char *outBuf;
  int len;
  z_stream stream;

  Telnet *telnet;
  bool usemccp;
  bool mccp1, mccp2;
  bool instream;
  char seqData[8];
  int state;

  int compressed, decompressed;
};

#endif
