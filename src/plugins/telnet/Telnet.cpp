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

#include "Telnet.h"

#include "MClientEngineEvent.h"
#include "MClientEvent.h"
#include "PluginManager.h"

#include <QApplication>
#include <QDebug>
#include <QStringList>
#include <QVariant>
#include <QTextCodec>

#include <stdio.h>

#include <algorithm>
using std::copy;
using std::string;

Q_EXPORT_PLUGIN2(telnet, Telnet)

struct cTelnetPrivate {
  /** socket */
  QString hostName;
  int hostPort;

  QString encoding;

  QTextCodec *codec;
  QTextDecoder *inCoder;
  QTextEncoder *outCoder;

#ifdef HAVE_MCCP
  /** object that handles MCCP */
  MCCP *_MCCP;
  bool usingmccp;
#endif

#ifdef HAVE_MSP
  /** object that handles MSP */
  cMSP *MSP;
  bool usingmsp;
#endif

#ifdef HAVE_MXP
  bool usingmxp, mxpNegotiated;
  int mxpallow;
#endif
 
  //iac: last char was IAC
  //iac2: last char was DO, DONT, WILL or WONT
  //insb: we're in IAC SB, waiting for IAC SE
  string command;
  bool iac, iac2, insb;
  
  /** are we connected? */
  bool _connected;
  
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
  bool t_cmdEcho;
  bool t_lpmudstyle;
  bool _startupneg;
  /** current dimensions */
  int curX, curY;
  /** offline connection */
  bool offLineConnection;

  QString termType;
};

#define DEFAULT_ENCODING "ISO 8859-1"

Telnet::Telnet(QObject* parent) 
        : MClientFilterPlugin(parent) {
    _shortName = "telnetdatafilter";
    _longName = "Telnet Data Filter";
    _description = "A telnet data filter.";
    // TODO
    _dependencies.insert("socketmanager", 10);
    _implemented.insert("telnet",1);
    _receivesDataTypes << "SocketData" << "SocketConnected"
		       << "SocketDisconnected";
    _deliversDataTypes << "TelnetData";

    /** KMuddy Telnet */
    d = new cTelnetPrivate;
    
    d->termType = "KMuddy";
    
    d->codec = 0;
    d->inCoder = 0;
    d->outCoder = 0;
    
    d->iac = d->iac2 = d->insb = false;
    d->command = "";
    
    d->sentbytes = 0;
    d->offLineConnection=false;
    d->_connected = false;
    d->curX = 0;
    d->curY = 0;
    d->_startupneg = false;
    d->encoding = DEFAULT_ENCODING;
    
    reset ();
}


Telnet::~Telnet() {
  delete d->inCoder;
  delete d->outCoder;

  delete d;
  d = 0;
}

void Telnet::customEvent(QEvent* e) {
  if (e->type() == 10000)
    engineEvent(e);
  else if (e->type() == 10001) {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    QString s;
    QStringList types = me->dataTypes();
    foreach(s, types) {
      if (s.startsWith("SocketData")) {
	socketRead(me->payload()->toByteArray());
	
      }
      else if(s.startsWith("SocketConnected")) {
	d->_connected = true;
	
	reset ();
	
	d->sentbytes = 0;
	
	    //negotiate some telnet options, if allowed
	if (d->_startupneg)
	  {
		//NAWS (used to send info about window size)
	    sendTelnetOption (TN_WILL, OPT_NAWS);
	    //do not allow server to echo our text!
	    sendTelnetOption (TN_DONT, OPT_ECHO);
	  }
	
	// set up encoding
	d->encoding = DEFAULT_ENCODING;
	// d->encoding = sett ? sett->getString ("encoding") : DEFAULT_ENCODING;
	setupEncoding ();
	
      }
      else if(s.startsWith("SocketDisconnected")) {
	d->_connected = false;
	
	reset ();
      }
      else if(s.startsWith("DimensionsChanged")) {
	QList<QVariant> par = me->payload()->toList();
	windowSizeChanged (par.first().toInt(), par.last().toInt());
      }
    }
  }
  else {
    qDebug() << "Telnet somehow received the wrong kind of event...";
    
  }
}


void Telnet::configure() {
}


bool Telnet::loadSettings() {
  return true;
}


bool Telnet::saveSettings() const {
  return true;
}


bool Telnet::startSession(QString) {
    return true;
}


bool Telnet::stopSession(QString) {
    return true;
}

void Telnet::setupEncoding ()
{
  delete d->inCoder;
  delete d->outCoder;

  d->codec = QTextCodec::codecForName (d->encoding.toLatin1().data());
  if (!d->codec) {  // unable to create codec - use latin1
    d->codec = QTextCodec::codecForName (DEFAULT_ENCODING);
  }
  d->inCoder = d->codec->makeDecoder ();
  d->outCoder = d->codec->makeEncoder ();
}

void Telnet::reset ()
{
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
  d->command = "";
}

bool Telnet::sendData (const QString &data)
{
  if (!(isConnected()))
    return false;
  // return true and dont send, since offline is used
  if (isOffLineConnection())
    return true;

  if (d->t_cmdEcho == true && d->t_lpmudstyle)
    d->prependGANewLine = false;

  string outdata = (d->outCoder->fromUnicode(data)).data();

  // IAC byte must be doubled
  int len = outdata.length();
  bool gotIAC = false;
  for (int i = 0; i < len; i++)
    if ((unsigned char) outdata[i] == TN_IAC) {
      gotIAC = true;
      break;
    }
  if (gotIAC) {
    string d;
    // double IACs
    for (int i = 0; i < len; i++)
    {
      d += outdata[i];
      if ((unsigned char) outdata[i] == TN_IAC)
        d += outdata[i];  //double IAC
    }
    outdata = d;
  }

  //data ready, send it
  return doSendData (outdata);
}

bool Telnet::doSendData (const string &data)
{
  if (!(isConnected()))
    return false;
  if (isOffLineConnection())
    return true;
  //write data to socket - it's so complicated because sometimes only a part of data
  //is accepted at a time
  int dataLength = data.length ();

//   QByteArray ba(data.c_str(), dataLength);
//   QVariant* qv = new QVariant(ba);
//   QStringList sl("SendToSocketData");  
//   // FIXME: HACK! :(
//   foreach(QString s, _runningSessions) {
//       postSession(qv, sl, s);
//   }

// HACK: This is BROKEN in the protocol, disabled.

  //update counter
  d->sentbytes += dataLength;
  return true;
}

void Telnet::windowSizeChanged (int x, int y)
{
  //remember the size - we'll need it if NAWS is currently disabled but will
  //be enabled. Also remember it if no connection exists at the moment;
  //we won't be called again when connecting
  d->curX = x;
  d->curY = y;
  if (!(isConnected()))
    return;
  if (d->myOptionState[OPT_NAWS])   //only if we have negotiated this option
  {
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
    doSendData (s);
  }
}

void Telnet::sendTelnetOption (unsigned char type, unsigned char option)
{
  qDebug() << "Sending Telnet Option: " << type << " " << option;
  string s;
  s = TN_IAC;
  s += (unsigned char) type;
  s += (unsigned char) option;
  doSendData (s);
}

void Telnet::processTelnetCommand (const string &command)
{
  unsigned char ch = command[1];
  unsigned char option;
  qDebug() << "Received Telnet Command: " << ch << " " << option;
  switch (ch) {
    case TN_AYT:
      doSendData ("I'm here! Please be more patient!\r\n");
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
              (option == OPT_TERMINAL_TYPE) || (option == OPT_NAWS))
                 //these options are supported; compression is handled
                 //separately
          {
            sendTelnetOption (TN_DO, option);
            d->hisOptionState[option] = true;
          }
#ifdef HAVE_MXP
          else
          if (option == OPT_MXP)
          {
            // allow or disallow, MXP depending on whether it's disabled
            sendTelnetOption ((d->mxpallow >= 2) ? TN_DO : TN_DONT, option);
            d->hisOptionState[option] = true;

            //MXP is now negotiated
            d->mxpNegotiated = true;
            if (d->mxpallow == 2)  //MXP: if negotiated
            {
              cMXPManager *mm = dynamic_cast<cMXPManager *>(object ("mxpmanager"));
              mm->setMXPActive (true);
              puts ("KMuddy: MXP enabled !");
            }
          }
#endif
#ifdef HAVE_MSP
          else
          if (option == OPT_MSP)
          {
            sendTelnetOption (TN_DO, option);
            d->hisOptionState[option] = true;

            //MSP is now enabled
            d->usingmsp = true;
            d->MSP->enableMSP ();
            puts ("KMuddy: MSP enabled !");
          }
#endif
#ifdef HAVE_MCCP
          else
          if ((option == OPT_COMPRESS) || (option == OPT_COMPRESS2))
          //these are handled separately, as they're a bit special
          {
            if ((option == OPT_COMPRESS) && (d->hisOptionState[OPT_COMPRESS2]))
            {
              //protocol says: reject MCCP v1 if you have previously accepted
              //MCCP v2...
              sendTelnetOption (TN_DONT, option);
              d->hisOptionState[option] = false;
              puts ("KMuddy: Rejecting MCCP v1, because v2 is already used !");
            }
            else
            {
              sendTelnetOption (TN_DO, option);
              d->hisOptionState[option] = true;
              //inform MCCP object about the change
              if ((option == OPT_COMPRESS)) {
                d->_MCCP->setMCCP1 (true);
                puts ("KMuddy: MCCP v1 enabled !");
              }
              else {
                d->_MCCP->setMCCP2 (true);
                puts ("KMuddy: MCCP v2 enabled !");
              }
            }
          }
#endif
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
#ifdef HAVE_MCCP
          //inform MCCP object about the change - won't cause problems in
          //MCCP - see cmccp.cpp for more info
          if ((option == OPT_COMPRESS)) {
            d->_MCCP->setMCCP1 (false);
            puts ("KMuddy: MCCP v1 disabled !");
          }
          if ((option == OPT_COMPRESS2)) {
            d->_MCCP->setMCCP2 (false);
            puts ("KMuddy: MCCP v1 disabled !");
          }
#endif
        }
        d->heAnnouncedState[option] = true;
      }
#ifdef HAVE_MSP
      if (option == OPT_MSP)
      {
        //MSP is now disabled
        d->usingmsp = false;
        d->MSP->disableMSP ();
        puts ("KMuddy: MSP disabled !");
      }
#endif
#ifdef HAVE_MXP
      if (option == OPT_MXP)
      {
        //MXP is now disabled
        cMXPManager *mm = dynamic_cast<cMXPManager *>(object ("mxpmanager"));
        mm->setMXPActive (false);
        puts ("KMuddy: MXP disabled !");
      }
#endif
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
              string s;
              s = TN_IAC;
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
              doSendData (s);
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
              string s;
              s = TN_IAC;
              s += TN_SB;
              s += OPT_TERMINAL_TYPE;
              s += TNSB_IS;
              s += d->termType.toLatin1().data();
              s += TN_IAC;
              s += TN_SE;
              doSendData (s);
            }
          }
        break;
        //other cmds should not arrive, as they were not negotiated.
        //if they do, they are merely ignored
      };
      break;
    //other commands are simply ignored (NOP and such, see .h file for list)
  };
}

void Telnet::socketRead (const QByteArray &ba)
{
  char data[32769]; //clean data after decompression
  int amount = ba.size();

//   //we'll need cProfileSettings later on
//   cProfileSettings *sett = settings();

  int datalen;
#ifdef HAVE_MCCP
  //MCCP will be handled first. This is done in a separate class, which
  //detects all MCCP-related codes itself. I know that this involves some
  //code duplication (two telnet option parsers), but it's needed because
  //telnet sequences may (and will) appear inside the compressed streams,
  //so it's best to get plain uncompressed data first. Another reason is
  //improper design of MCCP v1, which uses unterminated telnet subsequences.
  //This has been fixed in MCCP v2, but some MUDs may still use the old
  //version.

  d->_MCCP->prepareDecompression ((char*)ba.data(), data, amount, 32768);
  while ((datalen = d->_MCCP->uncompressNext ()) != -1)
#else
  strncpy(data, ba.data(), amount);
  datalen = amount;
#endif
  {

    //inform plug-ins about uncompressed data (can be equal to raw data if MCCP isn't used
//  invokeEvent ("raw-data", sess(), QString (data));

#if 0
printf ("DATA IN: ");
for (int i = 0; i < datalen; ++i) printf ("%02x ", (unsigned char) data[i]);
printf ("\n");
#endif

    data[datalen] = '\0';
    string cleandata;

    //clear the GO-AHEAD flag
    d->recvdGA = false;
    
    //now we have the data, but we cannot forward it to next stage of processing,
    //because the data contains telnet commands
    //so we parse the text and process all telnet commands:

    for (unsigned int i = 0; i < (unsigned int) datalen; i++)
    {
      unsigned char ch = data[i];
      if (d->iac || d->iac2 || d->insb || (ch == TN_IAC))
      {
        //there are many possibilities here:
        //1. this is IAC, previous character was regular data
        if (! (d->iac || d->iac2 || d->insb) && (ch == TN_IAC))
        {
          d->iac = true;
          d->command += ch;
        }
        else
        //2. seq. of two IACs
          if (d->iac && (ch == TN_IAC) && (!d->insb))
        {
          d->iac = false;
          cleandata += ch;
          d->command = "";
        }
        else
        //3. IAC DO/DONT/WILL/WONT
          if (d->iac && (!d->insb) &&
            ((ch == TN_WILL) || (ch == TN_WONT) || (ch == TN_DO) || (ch == TN_DONT)))
        {
          d->iac = false;
          d->iac2 = true;
          d->command += ch;
        }
        else
        //4. IAC DO/DONT/WILL/WONT <command code>
          if (d->iac2)
        {
          d->iac2 = false;
          d->command += ch;
          processTelnetCommand (d->command);
          d->command = "";
        }
        else
        //5. IAC SB
          if (d->iac && (!d->insb) && (ch == TN_SB))
        {
          d->iac = false;
          d->insb = true;
          d->command += ch;
        }
        else
        //6. IAC SE without IAC SB - error - ignored
          if (d->iac && (!d->insb) && (ch == TN_SE))
        {
          d->command = "";
          d->iac = false;
        }
        else
        //7. inside IAC SB
          if (d->insb)
        {
          d->command += ch;
          if (d->iac && (ch == TN_SE))  //IAC SE - end of subcommand
          {
            processTelnetCommand (d->command);
            d->command = "";
            d->iac = false;
            d->insb = false;
          }
          if (d->iac)
            d->iac = false;
          else
          if (ch == TN_IAC)
              d->iac = true;
        }
        else
        //8. IAC fol. by something else than IAC, SB, SE, DO, DONT, WILL, WONT
        {
          d->iac = false;
          d->command += ch;
          processTelnetCommand (d->command);
          //this could have set receivedGA to true; we'll handle that later
          // (at the end of this function)
          d->command = "";
        }
      }
      else   //plaintext
      {
        //everything except CRLF is okay; CRLF is replaced by LF(\n) (CR ignored)
        if (ch != 13)
          cleandata += ch;
      }

      // TODO: do something about all that code duplication ...

      //we've just received the GA signal - higher layers shall be informed about it
      if (d->recvdGA)
      {
#ifdef HAVE_MSP
        //ask MSP parser to look for MSP tags if MSP is enabled
        //look if the user want to have MSP all the time
        bool alwaysmsp = sett ? sett->getBool ("always-msp") : false;
        //hand data to MSP parser if desired
        if (d->usingmsp || alwaysmsp)
          cleandata = d->MSP->parseServerOutput (cleandata);
#endif

        //prepend a newline, if needed
        if (d->prependGANewLine && d->t_lpmudstyle)
          cleandata = "\n" + cleandata;
        d->prependGANewLine = false;
        //forward data for further processing
        QString unicodeData = d->inCoder->toUnicode (cleandata.data(), cleandata.length());

	QVariant* qv = new QVariant(unicodeData);
	QStringList sl;
	sl << "TelnetData";
    //FIXME: HACK! :(
        postSession(qv, sl);
	qDebug() << "posted FilteredData!";
	
//         invokeEvent ("data-received", sess(), unicodeData);

//         if (sett && sett->getBool ("prompt-console"))
//           //we'll need to prepend a new-line in next data sending
//           d->prependGANewLine = true;
//         //we got a prompt
//         invokeEvent ("received-ga", sess());

        //clean the flag, and the data (so that we don't send it multiple times)
        cleandata = "";
        d->recvdGA = false;
      }
    }

    //some data left to send - do it now!
    if (!cleandata.empty())
    {
#ifdef HAVE_MSP
      //ask MSP parser to look for MSP tags if MSP is enabled
      //look if the user want to have MSP all the time
      bool alwaysmsp = sett ? sett->getBool ("always-msp") : false;
      //hand data to MSP parser if desired
      if (d->usingmsp || alwaysmsp)
        cleandata = d->MSP->parseServerOutput (cleandata);
#endif

      //prepend a newline, if needed
      if (d->prependGANewLine && d->t_lpmudstyle)
        cleandata = "\n" + cleandata;
      d->prependGANewLine = false;
      //forward data for further processing
      QString unicodeData = d->inCoder->toUnicode (cleandata.data(), cleandata.length());
      QVariant* qv = new QVariant(unicodeData);
      QStringList sl;
      sl << "TelnetData";
      //FIXME: HACK! :(
      postSession(qv, sl);
      qDebug() << "posted FilteredData!";
//    invokeEvent ("data-received", sess(), unicodeData);
    }
  }

  //invokeEvent ("text-here", sess());
}

void Telnet::setOffLineConnection (bool type)
{
  d->offLineConnection = type;
}

/** are we connected (offline connection is counted as connected) ? */
bool Telnet::isConnected ()
{
  return (isOffLineConnection() || d->_connected);
}

bool Telnet::isOffLineConnection () { 
  return d->offLineConnection;
}

int Telnet::sentBytes () {
  return d->sentbytes;
}

void Telnet::setCommandEcho (bool cmdEcho)
{
  d->t_cmdEcho = cmdEcho;
}

void Telnet::setLPMudStyle (bool lpmudstyle)
{
  d->t_lpmudstyle = lpmudstyle;
}

void Telnet::setNegotiateOnStartup (bool startupneg)
{
  d->_startupneg = startupneg;
}
