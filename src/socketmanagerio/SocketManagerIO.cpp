#include "SocketManagerIO.h"

#include "SocketManagerIOConfig.h"
#include "SocketReader.h"

#include "MClientEvent.h"
#include "PluginManager.h"
#include "CommandManager.h"

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

    // SocketManager members
    _settingsFile = "config/"+_shortName+".xml";
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
        //qDebug() << "* ba.data() in SocketManagerIO:" << ba.data();
        qDebug() << "* me->payload() is" << me->payload();
	sendData(ba, me->session());

    } else if (me->dataTypes().contains("ConnectToHost")) {
        QString arg = me->payload()->toString();
        qDebug() << "* ConnectToHost arguments: " << arg;
        connectDevice(me->session());

    } else if (me->dataTypes().contains("DisconnectFromHost")) {
        QString arg = me->payload()->toString();
        qDebug() << "* DisconnectToHost arguments: " << arg;
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
    QIODevice* device = new QFile(_settingsFile);
    if(!device->open(QIODevice::ReadOnly)) {
        qCritical() << "Can't open file for reading:" << _settingsFile;
        return false;
    }

    QXmlStreamReader* xml = new QXmlStreamReader(device);
    QString profile;
    QPair<QString, QVariant> p;
    bool in_proxy = false;
    while(!xml->atEnd()) {
        xml->readNext();

        if(xml->isEndElement()) {
            if(xml->name() == "profile") {
                // found </profile>
            } else if(xml->name() == "proxy") {
                in_proxy = false;
            }

        } else if(xml->isStartElement()) {
            if(xml->name() == "config") {
                QXmlStreamAttributes attr = xml->attributes();
                QString version = attr.value("version").toString();
                if(version.toDouble() < _configVersion.toDouble()) {
                    qWarning() << "Config file is too old! Trying anyway...";                }

            } else if(xml->name() == "profile") {
                QXmlStreamAttributes attr = xml->attributes();
                profile = attr.value("name").toString();
                qDebug() << "* found profile:" << profile;

            } else if(xml->name() == "connection") {
                QXmlStreamAttributes attr = xml->attributes();
                QString host = attr.value("host").toString();
                QString port = attr.value("port").toString();
                p.first = "host";
                p.second = host;
                _settings.insert(profile, p);
                qDebug() << "* inserted host:" << host;
                p.first = "port";
                p.second = port;
                _settings.insert(profile, p);
                qDebug() << "* inserted port:" << port;
            
            } else if(xml->name() == "proxy") {
                QXmlStreamAttributes attr = xml->attributes();
                QString host = attr.value("host").toString();
                QString port = attr.value("port").toString();
                p.first = "proxy_host";
                p.second = host;
                _settings.insert(profile, p);
                qDebug() << "* inserted proxy_host:" << host;
                p.first = "proxy_port";
                p.second = port;
                _settings.insert(profile, p);
                qDebug() << "* inserted proxy_port:" << port;
                
                in_proxy = true;
            
            } else if(xml->name() == "username" && in_proxy == true) {
                QString proxy_user = xml->readElementText();
                p.first = "proxy_username";
                p.second = proxy_user;
                _settings.insert(profile, p);
                qDebug() << "* inserted proxy_username:" << proxy_user;
            
            } else if(xml->name() == "password" && in_proxy == true) {
                QString proxy_pass = xml->readElementText();
                p.first = "proxy_password";
                p.second = proxy_pass;
                _settings.insert(profile, p);
                qDebug() << "* inserted proxy_password:" << proxy_pass;
            }
        }
    }

    delete device;
    delete xml;
    
    return true;
}


const bool SocketManagerIO::saveSettings() const {
    QIODevice* device = new QFile(_settingsFile);
    if(!device->open(QIODevice::WriteOnly)) {
        qCritical() << "Can't open file for writing:" << _settingsFile;
        return false;
    }

    QXmlStreamWriter* xml = new QXmlStreamWriter(device);
    xml->setAutoFormatting(true);
    xml->writeStartDocument();
    xml->writeStartElement("config");
    xml->writeAttribute("version", _configVersion);

    foreach(QString s, _settings.uniqueKeys()) {
        xml->writeStartElement("profile");
        xml->writeAttribute("name", s);
        
        QPair<QString, QVariant> p;
        
        xml->writeEmptyElement("connection");
        foreach(p, _settings.values(s)) {
            if(p.first == "host") {
                xml->writeAttribute("host", p.second.toString());
            } else if(p.first == "port") {
                xml->writeAttribute("port", p.second.toString());
            }
        }

        xml->writeStartElement("proxy");
        foreach(p, _settings.values(s)) {
            if(p.first == "proxy_host") {
                xml->writeAttribute("host", p.second.toString());
            } else if(p.first == "proxy_port") {
                xml->writeAttribute("port", p.second.toString());
            }
        }
        
        foreach(p, _settings.values(s)) {
            if(p.first == "proxy_username") {
                xml->writeStartElement("username");
                xml->writeCharacters(p.second.toString());
                xml->writeEndElement();
            }
            else if(p.first == "proxy_password") {
                xml->writeStartElement("password");
                xml->writeCharacters(p.second.toString());
                xml->writeEndElement();
            }
        }
        xml->writeEndElement(); // proxy
        
        xml->writeEndElement(); // profile
    }

    xml->writeEndElement(); // config
    xml->writeEndDocument();
    qDebug() << "* wrote xml";

    delete device;
    delete xml;

    return true;
}


const bool SocketManagerIO::startSession(QString s) {
    
    QString host;
    int port = 0;
    QString proxy_host;
    int proxy_port = 0;
    QString proxy_user;
    QString proxy_pass;

    //qDebug() << _settings.values(s);
    
    QPair<QString, QVariant> p;
    foreach(p, _settings.values(s)) {
        if(p.first == "host") host = p.second.toString();
        else if(p.first == "port") port = p.second.toInt();
        else if(p.first == "proxy_host") {
            proxy_host = p.second.toString();
            qDebug() << "* proxy_host" << proxy_host;

        } else if(p.first == "proxy_port") proxy_port = p.second.toInt();
        else if(p.first == "proxy_username") { 
            proxy_host = p.second.toString();
        } else if(p.first == "proxy_password") {
            proxy_pass = p.second.toString();
        }
    }

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
    
    qDebug() << "* threads:" << sr->thread() << this->thread();
    sr->moveToThread(this->thread());
    qDebug() << "* threads:" << sr->thread() << this->thread();
    sr->host(host);
    sr->port(port);
    _socketReaders.insert(s, sr);
    _runningSessions << s;
    qDebug() << "* inserted SocketReader for session" << s;

    // register Commands for CommandManager
    QStringList commands;
    commands << _shortName
	     << "connect" << "ConnectToHost"
	     << "zap" << "DisconnectFromHost";
    CommandManager *cm = CommandManager::instance();
    cm->registerCommand(commands);

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
    qDebug() << "* SocketManagerIO thread:" << this->thread();
    // Attempts to connect every socket associated with the session s
    foreach(SocketReader* sr, _socketReaders.values(s)) {
        qDebug() << "* threads:" << sr->thread() << this->thread();
        sr->connectToHost();//"mume.org",4242);
        qDebug() << "* connected socket for session" << s;
    }
}


void SocketManagerIO::disconnectDevice(QString s) {
    // Disconnect a particular session's sockets.
    foreach(SocketReader* sr, _socketReaders.values(s)) {
        sr->closeSocket();
    }
}


void SocketManagerIO::sendData(const QByteArray& ba, const QString& session) {
    // Send data to the sockets.
    if (_openSockets.values(session).size() == 0) {

      displayMessage("#no open connections. Use '#connect' to open a connection.\n", session);

    } else {
      
      SocketReader* sr;
      foreach(sr, _openSockets.values(session)) {
        sr->sendToSocket(new QByteArray(ba.data()));
      }

    }
}


void SocketManagerIO::socketReadData(const QByteArray& data, const QString& s) {
    qDebug() << "received data from" << s;

    QVariant* qv = new QVariant(data);
    QStringList tags;
    tags << "SocketData";

    postEvent(qv, tags, s);
}

// Implementation-specific details: slots for successful operations

void SocketManagerIO::displayMessage(const QString& message, const QString& s) {
    QVariant* qv = new QVariant(message);
    QStringList sl;
    sl << "XMLDisplayData";
    postEvent(qv, sl, s);
}


void SocketManagerIO::socketOpened(SocketReader* sr) {
    _openSockets.insert(sr->session(), sr);

    QVariant* qv = new QVariant();
    QStringList sl;
    sl << "SocketConnected";
    postEvent(qv, sl, sr->session());
}

void SocketManagerIO::socketClosed(SocketReader* sr) {
    _openSockets.remove(sr->session(), sr);

    QVariant* qv = new QVariant();
    QStringList sl;
    sl << "SocketDisconnected";
    postEvent(qv, sl, sr->session());
}
