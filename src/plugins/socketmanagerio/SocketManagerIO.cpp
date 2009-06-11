#include "SocketManagerIO.h"
#include "SocketManagerIOConfig.h"
#include "SocketReader.h"

#include "MClientEvent.h"
#include "PluginManager.h"
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
    _dataTypes << "SendToSocketData" << "ConnectToHost"
	       << "DisconnectFromHost";
    _configurable = true;
    _configVersion = "2.0";

    // register commands
    QStringList commands;
    commands << _shortName
	     << "connect" << "ConnectToHost"
	     << "zap" << "DisconnectFromHost";
    CommandManager::instance()->registerCommand(commands);
}


SocketManagerIO::~SocketManagerIO() {
    // Clean up the QHash of sockets.
    stopAllSessions();
    saveSettings();
}


// MClientPlugin members
void SocketManagerIO::customEvent(QEvent* e) {
    if(!e->type() == 10001) return;
    
    MClientEvent* me;
    me = static_cast<MClientEvent*>(e);

    if(me->dataTypes().contains("SendToSocketData")) {
        QByteArray ba = me->payload()->toByteArray();
	sendData(ba, me->session());

    } else if (me->dataTypes().contains("ConnectToHost")) {
        QString arg = me->payload()->toString();
        connectDevice(me->session());

    } else if (me->dataTypes().contains("DisconnectFromHost")) {
        QString arg = me->payload()->toString();
        disconnectDevice(me->session());
    }

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


const bool SocketManagerIO::loadSettings() {
    _settings = *ConfigManager::instance()->pluginSettings(_shortName);
    return true;
}


const bool SocketManagerIO::saveSettings() const {
    ConfigManager::instance()->writePluginSettings(_shortName);
    return true;
}


const bool SocketManagerIO::startSession(QString s) {
    QString cfg = QString("config/%1/").arg(s);

    // Host settings
    QString host = _settings.value(cfg+"connection/host", "mume.org");
    int port = _settings.value(cfg+"connection/port", "4242").toInt();

    // Proxy settings
    QString proxy_host = _settings.value(cfg+"proxy/host", "proxy.example.com");
    int proxy_port = _settings.value(cfg+"proxy/port", "0").toInt();
    QString proxy_user = _settings.value(cfg+"proxy/proxy_user", "");
    QString proxy_pass = _settings.value(cfg+"proxy/proxy_pass", "");

    SocketReader* sr = new SocketReader(s, this);
    if(proxy_port != 0 && !proxy_host.isEmpty()) {
        QNetworkProxy* proxy = new QNetworkProxy();
        //proxy->setType(QNetworkProxy::Socks5Proxy);
        proxy->setHostName(proxy_host);
        proxy->setPort(proxy_port);
        proxy->setUser(proxy_user);
        proxy->setPassword(proxy_pass);
        sr->proxy(proxy);
        qDebug() << "* added proxy" << proxy_host << proxy_port
		 << "to SocketReader in session" << s;
    }
    //qDebug() << "* SockerReaderIO threads (sr, io):" << sr->thread() << this->thread();
    //sr->moveToThread(this->thread());
    //qDebug() << "* SockerReaderIO threads (sr, io):" << sr->thread() << this->thread();
    sr->host(host);
    sr->port(port);
    _socketReaders.insert(s, sr);
    _runningSessions << s;
    //qDebug() << "* inserted SocketReader for session" << s;

    return true;
}


const bool SocketManagerIO::stopSession(QString s) {
    foreach(SocketReader* sr, _socketReaders.values(s)) {
        delete sr;
        qDebug() << "* removed SocketReader for session" << s;
    }
    _socketReaders.remove(s);
    int removed = _runningSessions.removeAll(s);
    return removed!=0?true:false;
}


// IO members
void SocketManagerIO::connectDevice(QString s) {

    if (!_openSockets.values(s).isEmpty()) {
      displayMessage("#connection is already open. "
		     "Use '#zap' to disconnect it.\n", s);

    } else {
      // Connect a particular session's sockets.
      foreach(SocketReader* sr, _socketReaders.values(s))
        sr->connectToHost();

    }
}


void SocketManagerIO::disconnectDevice(QString s) {
    if (_openSockets.values(s).isEmpty()) {
      displayMessage("#no open connections to zap.\n", s);

    } else {
      // Disconnect a particular session's sockets.
      foreach(SocketReader* sr, _socketReaders.values(s))
        sr->closeSocket();
    }
}


void SocketManagerIO::sendData(const QByteArray& ba, const QString& session) {
    // Send data to the sockets.
    if (_openSockets.values(session).isEmpty()) {
      displayMessage("#no open connections. Use '#connect' to open a "
		     "connection.\n", session);

    } else {

      SocketReader* sr;
      foreach(sr, _openSockets.values(session)) {
        sr->sendToSocket(new QByteArray(ba.data()));
      }

    }
}


void SocketManagerIO::socketReadData(const QByteArray& data, const QString& s) {
    QVariant* qv = new QVariant(data);
    QStringList tags("SocketData");
    postEvent(qv, tags, s);
}

// Implementation-specific details: slots for successful operations

void SocketManagerIO::displayMessage(const QString& message, const QString& s) {
    QVariant* qv = new QVariant(message);
    QStringList sl("DisplayData");
    postEvent(qv, sl, s);
}


void SocketManagerIO::socketOpened(SocketReader *sr) {
    _openSockets.insert(sr->session(), sr);
    QVariant* qv = new QVariant();
    QStringList sl("SocketConnected");
    postEvent(qv, sl, sr->session());
}

void SocketManagerIO::socketClosed(SocketReader *sr) {
    _openSockets.remove(sr->session());
    QVariant* qv = new QVariant();
    QStringList sl("SocketDisconnected");
    postEvent(qv, sl, sr->session());
}
