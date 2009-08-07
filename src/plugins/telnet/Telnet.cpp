#include "Telnet.h"
#include "EventHandler.h"

Q_EXPORT_PLUGIN2(telnet, Telnet)

Telnet::Telnet(QObject* parent) 
        : MClientFilterPlugin(parent) {
    _shortName = "telnetdatafilter";
    _longName = "Telnet Data Filter";
    _description = "A telnet data filter.";
    _dependencies.insert("socketmanager", 10);
    _implemented.insert("telnet",1);
    _receivesDataTypes << "SocketReadData" << "SocketConnected"
		       << "SocketDisconnected" << "SocketWriteData";
    _deliversDataTypes << "TelnetData" << "TelnetGA" << "SendToSocketData"
		       << "EchoMode";
}


Telnet::~Telnet() {
}


bool Telnet::loadSettings() {
  return true;
}


bool Telnet::saveSettings() const {
  return true;
}


void Telnet::configure() {
}


bool Telnet::startSession(QString s) {
  _eventHandlers[s] = new EventHandler;
  return true;
}


bool Telnet::stopSession(QString s) {
  delete _eventHandlers[s];
  return true;
}


MClientEventHandler* Telnet::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}
