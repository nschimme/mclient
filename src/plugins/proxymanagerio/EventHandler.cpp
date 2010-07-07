#include "EventHandler.h"

#include <QEvent>
#include <QVariant>
#include <QCoreApplication> // for sending command

#include "MClientEvent.h"
#include "ProxyServer.h"
#include "ProxyConnection.h"

#include "AbstractPluginSession.h"
#include "CommandProcessor.h"
#include "ConfigEntry.h"

EventHandler::EventHandler(AbstractPluginSession *ps, MClientPlugin *mp)
  : MClientEventHandler(ps, mp) {
  _proxyServer = new ProxyServer(this);

  // Host settings
  QString password = _config->value("config/proxy/password", "").toString();
  int port = _config->value("config/proxy/port", "4243").toInt();

  _proxyServer->password(password.toAscii());
  _proxyServer->port(port);
  _proxyServer->start();

}


EventHandler::~EventHandler() {
  _proxyServer->deleteLater();
}


void EventHandler::customEvent(QEvent *e) {
  if (e->type() == 10001) {

    // Forward the event to the next in the chain
    forwardEvent(e);
    
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    if (me->dataTypes().contains("DisplayData") ||
	me->dataTypes().contains("UserInput")) {
      QByteArray ba = me->payload()->toByteArray();
      emit sendToSocket(ba);

    } else if (me->dataTypes().contains("DisplayPrompt")) {
      QByteArray ba = me->payload()->toByteArray();
      // Append the IAC GA
      ba += (unsigned char) 255; // IAC
      ba += (unsigned char) 249; // GA
      emit sendToSocket(ba);
      
    } else if (me->dataTypes().contains("EchoMode")) {
      echoMode(me->payload()->toBool());

    } else if (me->dataTypes().contains("SocketDisconnected")) {
      echoMode(true);

    } else if (me->dataTypes().contains("ProxyCommand")) {
      QString args = me->payload()->toString();
      proxyCommand(args);

    }
    
  }
  else 
    qDebug() << "SocketManagerIO got a customEvent of type" << e->type();
}


void EventHandler::sendUserInput(const QByteArray &input) {
  // TODO: post to the display directly? If we try posting the normal
  // way  we get loopback
  
  // Post to command processor
  QVariant *payload = new QVariant(input.trimmed());
  QStringList tags("UserInput");
  MClientEventData *med = new MClientEventData(payload, tags,
					       _pluginSession->session());
  MClientEvent* me = new MClientEvent(med);
  QCoreApplication::postEvent(_pluginSession->getCommand(), me);
  
}


bool EventHandler::proxyCommand(const QString &args) {
  const ProxyConnections map = _proxyServer->getConnections();

  if (args.isEmpty()) {
    unsigned int j = 1;
    QString message("#\tIP\tConnection\r\n");
    ProxyConnections::const_iterator i = map.constBegin();
    while (i != map.constEnd()) {
      QString state;
      switch (i.value()->state()) {
      case PROXY_AUTHENTICATING:
	state = "Authenticating";
	break;
      case PROXY_CONNECTED:
	state = "Connected";
	break;
      case PROXY_DISCONNECTED:
      case PROXY_FAILED_PASSWORD:
	state = "Disconnected";
	break;
      };
      message += QString("%1\t%2\t%3\t%4\r\n")
	.arg(QString::number(j++))
	.arg(i.value()->peerAddress())
	.arg(i.key().toString())
	.arg(state);
      ++i;
    }
    displayMessage(message);
    return true;

  } else {
    // Other commands
    QRegExp rx("^(kill|listen|close)(?:\\s+(\\d+))?");
    if (!rx.exactMatch(args)) {
      // Incorrect command syntax
      displayMessage("#usage:\r\n"
		     "\t#proxy listen\r\n"
		     "\t#proxy close\r\n"
		     "\t#proxy kill #\r\n");
      return false;
    }
    // Parse the command
    QStringList cmd = rx.capturedTexts();
    
    qDebug() << cmd;
    QString command(cmd.at(1));
    QString number(cmd.at(2));

    if (command == "listen") {
      _proxyServer->start();

    } else if (command == "close") {
      _proxyServer->close();

    } else if (command == "kill") {
      unsigned int j = 1;
      ProxyConnections::const_iterator i = map.constBegin();
      while (i != map.constEnd()) {
	if (j++ == number.toUInt()) {
	  emit removeProxyConnection(i.value());
	  i = map.constEnd();

	} else ++i;
      }
    }
    return true;
  }
}


void EventHandler::echoMode(bool b) {
  QByteArray ba;
  ba += (unsigned char) 255; // IAC
  if (b) ba += (unsigned char) 251; // WILL
  else ba += (unsigned char) 252; // WON'T
  ba += (unsigned char) 1;   // OPT_ECHO
  emit sendToSocket(ba);
}


void EventHandler::displayMessage(const QString &message) {
  QVariant* qv = new QVariant(message);
  QStringList sl("DisplayData");
  postSession(qv, sl);
}
