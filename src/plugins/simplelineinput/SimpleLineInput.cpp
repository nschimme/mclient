#include <QApplication>
#include <QDebug>
#include <QVariant>

#include "SimpleLineInput.h"
#include "PluginSession.h"
#include "EventHandler.h"

Q_EXPORT_PLUGIN2(simplelineinput, SimpleLineInput)


SimpleLineInput::SimpleLineInput(QObject* parent) 
        : MClientPlugin(parent) {
    _shortName = "simplelineinput";
    _longName = "Simple Line Input";
    _description = "A simple line input plugin.";
    //_dependencies.insert("terrible_test_api", 1);
//    _implemented.insert("some_other_api",1);
    _receivesDataTypes << "EchoMode"  << "SocketDisconnected";
    _configurable = false;
}


SimpleLineInput::~SimpleLineInput() {
}


void SimpleLineInput::configure() {
}


bool SimpleLineInput::startSession(PluginSession *ps) {
  QString s = ps->session();
  qDebug() << "* starting SimpleLineInput for session" << s;
  _eventHandlers[s] = new EventHandler(ps, this);
  return true;
}


bool SimpleLineInput::stopSession(PluginSession *ps) {
  QString s = ps->session();
  qDebug() << "* removed Simple DisplayWidget for session" << s << this;
  _eventHandlers[ps->session()]->deleteLater();
  _eventHandlers.remove(ps->session());
  return true;
}


MClientEventHandler* SimpleLineInput::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}
