#include "Telnet.h"

#include "PluginSession.h"
#include "EventHandler.h"
#include <QDebug>

Q_EXPORT_PLUGIN2(telnet, Telnet)

Telnet::Telnet(QObject* parent) 
        : MClientPlugin(parent) {
    _shortName = "telnetdatafilter";
    _longName = "Telnet Data Filter";
    _description = "A telnet data filter.";
    _dependencies.insert("socketmanager", 10);
    _implemented.insert("telnet",1);
    _receivesDataTypes.insert("SocketWriteData", 3);
    _receivesDataTypes.insert("SocketReadData", 3);
    _receivesDataTypes.insert("SocketConnected", 3);
    _receivesDataTypes.insert("SocketDisconnected", 3);
    /*
    _deliversDataTypes << "TelnetData" << "TelnetGA" << "SendToSocketData"
		       << "EchoMode";
    */
}


Telnet::~Telnet() {
  qDebug() << "~Telnet";
}


void Telnet::configure() {
}


bool Telnet::startSession(PluginSession *ps) {
  _eventHandlers[ps->session()] = new EventHandler(ps, this);
  return true;
}


bool Telnet::stopSession(PluginSession *ps) {
  _eventHandlers[ps->session()]->deleteLater();
  _eventHandlers.remove(ps->session());
  return true;
}


MClientEventHandler* Telnet::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}
