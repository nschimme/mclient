/***************************************************************************
                          Telnet.cpp  -  handles telnet connection
    begin                : Pi Jun 14 2002
    copyright            : (C) 2002-2008 by Tomas Mecir
    email                : kmuddy@kmuddy.com

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

#include "TelnetParser.h"
#include "EventHandler.h"
#include "MpiParser.h"

#include <QDebug>
#include <QStringList>
#include <QTextCodec>
#include <QApplication> // for beep

struct cTelnetPrivate {
  QString encoding;

  QTextCodec *codec;
  QTextDecoder *inCoder;
  QTextEncoder *outCoder;

  //iac: last char was IAC
  //iac2: last char was DO, DONT, WILL or WONT
  //insb: we're in IAC SB, waiting for IAC SE
  QByteArray command;
  bool iac, iac2, insb;
  
  /** current state of options on our side and on server side */
  bool myOptionState[256], hisOptionState[256];
  /** whether we have announced WILL/WON'T for that option (if we have, we don't
      respond to DO/DON'T sent by the server -- see implementation and RFC 854
      for more information... */
  bool announcedState[256];
  /** whether the server has already announced his WILL/WON'T */
  bool heAnnouncedState[256];
  /** whether we have tried to enable this option */
  bool triedToEnable[256];
  /** amount of bytes sent up to now */
  int sentbytes;
  /** have we received the GA signal? */
  bool recvdGA;
  /** should we prepend newline after receving a GA */
  bool prependGANewLine;
  bool echoMode;
  bool startupneg;
  /** current dimensions */
  int curX, curY;

  /* Terminal Type */
  QString termType;
};

#define DEFAULT_ENCODING "ISO 8859-1"

TelnetParser::TelnetParser(EventHandler *eh)
  : QObject(eh), _eventHandler(eh) {

  connect(_eventHandler, SIGNAL(sendData(const QByteArray &)), SLOT(unIACData(const QByteArray &)));
  connect(_eventHandler, SIGNAL(readData(const QByteArray &)),SLOT(readData(const QByteArray &)));
  connect(_eventHandler, SIGNAL(windowSizeChanged(int, int)), SLOT(windowSizeChanged(int, int)));
  connect(_eventHandler, SIGNAL(socketConnected()), SLOT(socketConnected()));

  connect(this, SIGNAL(socketWrite(const QByteArray &)), _eventHandler, SLOT(socketWrite(const QByteArray &)));
  connect(this, SIGNAL(echoModeChanged(bool)), _eventHandler, SLOT(echoModeChanged(bool)));
  connect(this, SIGNAL(displayData(const QString &, bool)), _eventHandler, SLOT(displayData(const QString &, bool)));

  /** KMuddy Telnet */
  d = new cTelnetPrivate;  
  d->termType = "mClient";  
  // set up encoding
  d->codec = 0;
  d->inCoder = 0;
  d->outCoder = 0;
  d->iac = d->iac2 = d->insb = false;
  d->command.clear();
  d->sentbytes = 0;
  d->curX = 125;
  d->curY = 39;
  d->startupneg = false;
  d->encoding = DEFAULT_ENCODING;
  reset();
  setupEncoding();

  // Let's set up a nice buffer that is pretty big
  _cleanData.resize(32769);
  _cleanData.clear();

  // Set up our MPI Parser
  _mpiParser = new MpiParser();
}


TelnetParser::~TelnetParser() {
  delete d->inCoder;
  delete d->outCoder;

  delete d;
  d = 0;

  delete _mpiParser;
}


void TelnetParser::socketConnected() {
  qDebug() << "Telnet detected socket connect!";
	
  reset ();	
  d->sentbytes = 0;
	
  //negotiate some telnet options, if allowed
  if (d->startupneg) {
    //NAWS (used to send info about window size)
    sendTelnetOption (TN_WILL, OPT_NAWS);
    //do not allow server to echo our text!
    sendTelnetOption (TN_DONT, OPT_ECHO);
    //we will send our terminal type
    sendTelnetOption (TN_WILL, OPT_TERMINAL_TYPE);
  }
	
}

void TelnetParser::setupEncoding() {
  delete d->inCoder;
  delete d->outCoder;

  d->codec = QTextCodec::codecForName (d->encoding.toLatin1().data());
  if (!d->codec) {  // unable to create codec - use latin1
    d->codec = QTextCodec::codecForName (DEFAULT_ENCODING);
  }
  d->inCoder = d->codec->makeDecoder ();
  d->outCoder = d->codec->makeEncoder ();
}

void TelnetParser::reset() {
  //prepare option variables
  for (int i = 0; i < 256; i++)
  {
    d->myOptionState[i] = false;
    d->hisOptionState[i] = false;
    d->announcedState[i] = false;
    d->heAnnouncedState[i] = false;
    d->triedToEnable[i] = false;
  }
  //reset telnet status
  d->iac = d->iac2 = d->insb = false;
  d->command.clear();
}

void TelnetParser::unIACData(const QByteArray &data) {
  if (d->echoMode)
    d->prependGANewLine = false;

  QByteArray outdata = d->outCoder->fromUnicode(data);

  // IAC byte must be doubled
  int len = outdata.length();
  bool gotIAC = false;
  for (int i = 0; i < len; i++)
    if ((unsigned char) outdata[i] == TN_IAC) {
      gotIAC = true;
      break;
    }
  if (gotIAC) {
    QByteArray d;
    // double IACs
    for (int i = 0; i < len; i++) {
      d.append(outdata.at(i));
      if ((unsigned char) outdata.at(i) == TN_IAC)
        d.append(outdata.at(i));  //double IAC
    }
    outdata = d;
  }

  //data ready, send it
  sendRawData(outdata);
}

bool TelnetParser::sendRawData (const QByteArray &data) {
  //update counter
  d->sentbytes += data.length();
  emit socketWrite(data);
  return true;
}

void TelnetParser::windowSizeChanged (int x, int y)
{
  //remember the size - we'll need it if NAWS is currently disabled but will
  //be enabled. Also remember it if no connection exists at the moment;
  //we won't be called again when connecting
  d->curX = x;
  d->curY = y;
  if (d->myOptionState[OPT_NAWS])   //only if we have negotiated this option
    {
      /*
    string s;
    s = TN_IAC;
    s += TN_SB;
    s += OPT_NAWS;
    unsigned char x1, x2, y1, y2;
    x1 = (unsigned char) x / 256;
    x2 = (unsigned char) x % 256;
    y1 = (unsigned char) y / 256;
    y2 = (unsigned char) y % 256;
    //IAC must be doubled
    s += x1;
    if (x1 == TN_IAC)
      s += TN_IAC;
    s += x2; 
    if (x2 == TN_IAC)
      s += TN_IAC;
    s += y1;
    if (y1 == TN_IAC)
      s += TN_IAC;
    s += y2;
    if (y2 == TN_IAC)
      s += TN_IAC;
    
    s += TN_IAC;
    s += TN_SE;
    sendRawData(s);
      */
  }
}

void TelnetParser::sendTelnetOption (unsigned char type, unsigned char option)
{
  qDebug() << "* Sending Telnet Command: " << type << " " << option;
  QByteArray s;
  s += TN_IAC;
  s += (unsigned char) type;
  s += (unsigned char) option;
  sendRawData(s);
}

void TelnetParser::processTelnetCommand (const QByteArray &command) {
  unsigned char ch = command[1];
  unsigned char option;

  switch (command.length()) {
  case 2:
    qDebug() << "* Processing Telnet Command:" << command[1];

    switch (ch) {
    case TN_AYT:
      sendRawData ("I'm here! Please be more patient!\r\n");
          //well, this should never be executed, as the response would probably
          //be treated as a command. But that's server's problem, not ours...
          //If the server wasn't capable of handling this, it wouldn't have
          //sent us the AYT command, would it? Impatient server = bad server.
          //Let it suffer! ;-)
      break;
    case TN_GA:
      d->recvdGA = true;
      //signal will be emitted later
      break;
    };
    break;

  case 3:
    qDebug() << "* Processing Telnet Command:" << command[1] << command[2];

    switch (ch) {
    case TN_WILL:
      //server wants to enable some option (or he sends a timing-mark)...
      option = command[2];

      d->heAnnouncedState[option] = true;
      if (d->triedToEnable[option])
      {
        d->hisOptionState[option] = true;
        d->triedToEnable[option] = false;
      }
      else
      {
        if (!d->hisOptionState[option])
            //only if this is not set; if it's set, something's wrong wth the server
            //(according to telnet specification, option announcement may not be
            //unless explicitly requested)
        {
          if ((option == OPT_SUPPRESS_GA) || (option == OPT_STATUS) ||
              (option == OPT_TERMINAL_TYPE) || (option == OPT_NAWS) ||
	      (option == OPT_ECHO))
                 //these options are supported
          {
            sendTelnetOption (TN_DO, option);
            d->hisOptionState[option] = true;
	    // Echo mode support
	    if (option == OPT_ECHO) {
	      d->echoMode = false;
	      emit echoModeChanged(d->echoMode);
	    }
          }
          else
          {
            sendTelnetOption (TN_DONT, option);
            d->hisOptionState[option] = false;
          }
        }
      }
      break;
    case TN_WONT:
      //server refuses to enable some option...
      option = command[2];
      if (d->triedToEnable[option])
      {
        d->hisOptionState[option] = false;
        d->triedToEnable[option] = false;
        d->heAnnouncedState[option] = true;
      }
      else
      {
        //send DONT if needed (see RFC 854 for details)
        if (d->hisOptionState[option] || (!d->heAnnouncedState[option]))
        {
          sendTelnetOption (TN_DONT, option);
          d->hisOptionState[option] = false;
	  if (option == OPT_ECHO) {
	    d->echoMode = true;
	    emit echoModeChanged(d->echoMode);
	  }
        }
        d->heAnnouncedState[option] = true;
      }
      break;
    case TN_DO:
      //server wants us to enable some option
      option = command[2];
      if (option == OPT_TIMING_MARK)
      {
        //send WILL TIMING_MARK
        sendTelnetOption (TN_WILL, option);
      }
      else if (!d->myOptionState[option])
      //only if the option is currently disabled
      {
        if ((option == OPT_SUPPRESS_GA) || (option == OPT_STATUS) ||
            (option == OPT_TERMINAL_TYPE) || (option == OPT_NAWS))
        {
          sendTelnetOption (TN_WILL, option);
          d->myOptionState[option] = true;
          d->announcedState[option] = true;
        }
        else
        {
          sendTelnetOption (TN_WONT, option);
          d->myOptionState[option] = false;
          d->announcedState[option] = true;
        }
      }
      if (option == OPT_NAWS)  //NAWS here - window size info must be sent
        windowSizeChanged (d->curX, d->curY);
      break;
    case TN_DONT:
      //only respond if value changed or if this option has not been announced yet
      option = command[2];
      if (d->myOptionState[option] || (!d->announcedState[option]))
      {
        sendTelnetOption (TN_WONT, option);
        d->announcedState[option] = true;
      }
      d->myOptionState[option] = false;
      break;
    };
    break;

  case 4:
    qDebug() << "* Processing Telnet Command:" << command[1] << command[2]
	     << command[3];

    switch (ch) {
    case TN_SB:
      //subcommand - we analyze and respond...
      option = command[2];
      switch (option) {
        case OPT_STATUS:
          //see OPT_TERMINAL_TYPE for explanation why I'm doing this
          if (true /*myOptionState[OPT_STATUS]*/)
          {
            if (command[3] == TNSB_SEND)
            //request to send all enabled commands; if server sends his
            //own list of commands, we just ignore it (well, he shouldn't
            //send anything, as we do not request anything, but there are
            //so many servers out there, that you can never be sure...)
            {
              QByteArray s;
              s += TN_IAC;
              s += TN_SB;
              s += OPT_STATUS;
              s += TNSB_IS;
              for (int i = 0; i < 256; i++)
              {
                if (d->myOptionState[i])
                {
                  s += TN_WILL;
                  s += (unsigned char) i;
                }
                if (d->hisOptionState[i])
                {
                  s += TN_DO;
                  s += (unsigned char) i;
                }
              }
              s += TN_IAC;
              s += TN_SE;
              sendRawData (s);
            }
          }
        break;
        case OPT_TERMINAL_TYPE:
          if (d->myOptionState[OPT_TERMINAL_TYPE])
          {
            if (command[3] == TNSB_SEND)
              //server wants us to send terminal type; he can send his own type
              //too, but we just ignore it, as we have no use for it...
            {
              QByteArray s;
	      s += TN_IAC;
              s += TN_SB;
              s += OPT_TERMINAL_TYPE;
              s += TNSB_IS;
              s += d->termType.toLatin1().data();
              s += TN_IAC;
              s += TN_SE;
              sendRawData (s);
            }
          }
        break;
        //other cmds should not arrive, as they were not negotiated.
        //if they do, they are merely ignored
      };
      break;
    };
    break;
  };
  //other commands are simply ignored (NOP and such, see .h file for list)
}

void TelnetParser::readData (const QByteArray &data) {
  //clear the GO-AHEAD flag
  d->recvdGA = false;
    
  //now we have the data, but we cannot forward it to next stage of processing,
  //because the data contains telnet commands
  //so we parse the text and process all telnet commands:
  
  for (unsigned int i = 0; i < (unsigned int) data.length(); i++) {
    if (d->iac || d->iac2 || d->insb ||
	((unsigned char) data.at(i) == TN_IAC)) {
      //there are many possibilities here:
      //1. this is IAC, previous character was regular data
      if (! (d->iac || d->iac2 || d->insb) &&
	  ((unsigned char) data.at(i) == TN_IAC)) {
	d->iac = true;
	d->command.append(data.at(i));
      }
      //2. seq. of two IACs
      else if (d->iac && ((unsigned char) data.at(i) == TN_IAC)
	       && (!d->insb)) {
	d->iac = false;
	_cleanData.append(data.at(i));
	d->command.clear();
      }
      //3. IAC DO/DONT/WILL/WONT
      else if (d->iac && (!d->insb) &&
	       (((unsigned char) data.at(i) == TN_WILL) ||
		((unsigned char) data.at(i) == TN_WONT) ||
		((unsigned char) data.at(i) == TN_DO)   ||
		((unsigned char) data.at(i) == TN_DONT))) {
	d->iac = false;
	d->iac2 = true;
	d->command.append(data.at(i));
      }
      //4. IAC DO/DONT/WILL/WONT <command code>
      else if (d->iac2) {
	d->iac2 = false;
	d->command.append(data.at(i));
	processTelnetCommand (d->command);
	d->command.clear();
      }
      //5. IAC SB
      else if (d->iac && (!d->insb) &&
	       ((unsigned char) data.at(i) == TN_SB)) {
	d->iac = false;
	d->insb = true;
	d->command.append(data.at(i));
      }
      //6. IAC SE without IAC SB - error - ignored
      else if (d->iac && (!d->insb) &&
	       ((unsigned char) data.at(i) == TN_SE)) {
	d->command.clear();
	d->iac = false;
      }
      //7. inside IAC SB
      else if (d->insb) {
	d->command.append(data.at(i));
	if (d->iac && //IAC SE - end of subcommand
	    ((unsigned char) data.at(i) == TN_SE)) {
	  processTelnetCommand (d->command);
	  d->command.clear();
	  d->iac = false;
	  d->insb = false;
	}
	if (d->iac)
	  d->iac = false;
	else if ((unsigned char) data.at(i) == TN_IAC)
	  d->iac = true;
      }
      //8. IAC fol. by something else than IAC, SB, SE, DO, DONT, WILL, WONT
      else {
	d->iac = false;
	d->command.append(data.at(i));
	processTelnetCommand (d->command);
	//this could have set receivedGA to true; we'll handle that later
	// (at the end of this function)
	d->command.clear();
      }
    }
    else {  //plaintext
      //everything except CRLF is okay; CRLF is replaced by LF(\n) (CR ignored)
        
      switch (data.at(i)) {
      case '\a': // BEL
	QApplication::beep();
	break;
      default:
	if (!_mpiParser->isMpi(data.at(i), _cleanData))
	  _cleanData.append(data.at(i));
      };
    }
    
    // TODO: do something about all that code duplication ...
    
    //we've just received the GA signal - higher layers shall be informed about it
    if (d->recvdGA) {
      //prepend a newline, if needed
      if (d->prependGANewLine) {
	_cleanData.prepend("\r\n");
	d->prependGANewLine = false;
      }

      //forward data for further processing
      QString unicodeData;
      unicodeData = d->inCoder->toUnicode(_cleanData);	
      emit displayData(unicodeData, true); // with GO-AHEAD
      
      //we'll need to prepend a new-line in next data sending
      d->prependGANewLine = true;
      //we got a prompt
      
      //clean the flag, and the data (so that we don't send it multiple times)
      _cleanData.clear();
      d->recvdGA = false;
    }
  }

  //some data left to send - do it now!
  if (!_cleanData.isEmpty()) {
    d->prependGANewLine = false;
    
    QString unicodeData = d->inCoder->toUnicode(_cleanData);
    emit displayData(unicodeData, false); // without GO-AHEAD

    // clean the buffer
    _cleanData.clear();
    
  }
}
