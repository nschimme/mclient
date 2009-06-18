#include "SocketManagerIO.h"
#include "SocketManagerIOConfig.h"
#include "SocketReader.h"

#include "MClientEvent.h"
#include "MClientEngineEvent.h"
#include "PluginManager.h"
#include "PluginSession.h"
#include "CommandManager.h"
#include "ConfigManager.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QtXml>

Q_EXPORT_PLUGIN2(socketmanagerio, SocketManagerIO)


SocketManagerIO::SocketManagerIO(QObject* parent) 
        : MClientIOPlugin(parent) {
    
    _shortName = "socketmanagerio";
    _longName = "SocketManager";
    _description = "A socket plugin that reads from sockets and inserts the data back into the stream.";
    //    _dependencies.insert("commandmanager", 10);
    _implemented.insert("socketmanager",10);
    _receivesDataTypes << "SendToSocketData" << "ConnectToHost"
		       << "DisconnectFromHost";
    _deliversDataTypes << "SocketReadData" << "SocketConnected"
		       << "SocketDisconnected" << "DisplayData";
    _configurable = true;
    _configVersion = "2.0";

    _openSocket = false;
}


SocketManagerIO::~SocketManagerIO() {
    // Clean up the QHash of sockets.
    //saveSettings();
}


// MClientPlugin members
void SocketManagerIO::customEvent(QEvent* e) {
  if (e->type() == 10000)
    engineEvent(e);
  else if (e->type() == 10001) {
    
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    if(me->dataTypes().contains("SendToSocketData")) {
      QByteArray ba = me->payload()->toByteArray();
      sendData(ba);
      
    } else if (me->dataTypes().contains("ConnectToHost")) {
      //QString arg = me->payload()->toString();
      connectDevice();
      
    } else if (me->dataTypes().contains("DisconnectFromHost")) {
      //QString arg = me->payload()->toString();
      disconnectDevice();
    }
  }
  else 
    qDebug() << "SocketManagerIO got a customEvent of type" << e->type();
}

void SocketManagerIO::configure() {
    // Need to display a table of (identifier, host, port)
    // Selecting one and pushing an "Edit..." button will bring up a dialog
    // with three QLineEdits, one for each field.  Closing that will save the
    // data to a member variable and update the table.
    
    // It will originally be populated by QSettings.

    if(!_configWidget) _configWidget = new SocketManagerIOConfig();
    if(!_configWidget->isVisible()) _configWidget->show();

}


bool SocketManagerIO::loadSettings() {
  _settings =
    _pluginSession->getManager()->getConfig()->pluginSettings(_shortName);
  
  // register commands
  QStringList commands;
  commands << _shortName
	   << "connect" << "ConnectToHost"
	   << "zap" << "DisconnectFromHost";
  
  _pluginSession->getManager()->getCommand()->registerCommand(commands);
  
  return true;
}


bool SocketManagerIO::saveSettings() const {
  _pluginSession->getManager()->getConfig()->writePluginSettings(_shortName);
  return true;
}


bool SocketManagerIO::startSession(QString s) {
    QString cfg = QString("config/%1/").arg(s);

    // Host settings
    QString host = _settings->value(cfg+"connection/host", "mume.org");
    int port = _settings->value(cfg+"connection/port", "4242").toInt();

    // Proxy settings
    QString proxy_host = _settings->value(cfg+"proxy/host", "proxy.example.com");
    int proxy_port = _settings->value(cfg+"proxy/port", "0").toInt();
    QString proxy_user = _settings->value(cfg+"proxy/proxy_user", "");
    QString proxy_pass = _settings->value(cfg+"proxy/proxy_pass", "");

    _socketReader = new SocketReader(s, this);
    if(proxy_port != 0 && !proxy_host.isEmpty()) {
        QNetworkProxy* proxy = new QNetworkProxy();
        //proxy->setType(QNetworkProxy::Socks5Proxy);
        proxy->setHostName(proxy_host);
        proxy->setPort(proxy_port);
        proxy->setUser(proxy_user);
        proxy->setPassword(proxy_pass);
        _socketReader->proxy(proxy);
        qDebug() << "* added proxy" << proxy_host << proxy_port
		 << "to SocketReader in session" << s;
    }
    _socketReader->host(host);
    _socketReader->port(port);

    return true;
}


bool SocketManagerIO::stopSession(QString s) {
  delete _socketReader;
  qDebug() << "* removed SocketReader for session" << s;
  return true;
}


// IO members
void SocketManagerIO::connectDevice() {
    if (_openSocket) {
      displayMessage("#connection is already open. "
		     "Use '#zap' to disconnect it.\n");

    } else {
      // Connect a particular session's sockets.
      emit connectToHost();

    }
}


void SocketManagerIO::disconnectDevice() {
    if (!_openSocket) {
      displayMessage("#no open connections to zap.\n");

    } else {
      // Disconnect a particular session's sockets.
      emit closeSocket();
    }
}


void SocketManagerIO::sendData(const QByteArray& ba) {
  // Send data to the sockets.
  if (!_openSocket) {
    displayMessage("#no open connections. Use '#connect' to open a "
		   "connection.\n");
    
  }
  else {
    // TODO: Why doesn't this work?
    //emit sendToSocket(ba);
    _socketReader->sendToSocket(ba);
  }
}


void SocketManagerIO::socketReadData(const QByteArray& data) {
  QVariant* qv = new QVariant(data);
  QStringList tags("SocketReadData");
  postSession(qv, tags);
}

// Implementation-specific details: slots for successful operations
void SocketManagerIO::displayMessage(const QString& message) {
  QVariant* qv = new QVariant(message);
  QStringList sl("DisplayData");
  postSession(qv, sl);
}


void SocketManagerIO::socketOpened() {
  _openSocket = true;
  QVariant* qv = new QVariant();
  QStringList sl("SocketConnected");
  postSession(qv, sl);
}

void SocketManagerIO::socketClosed() {
  _openSocket = false;
  QVariant* qv = new QVariant();
  QStringList sl("SocketDisconnected");
  postSession(qv, sl);
}
