#include "SimpleDisplay.h"
#include "EventHandler.h"

#include "PluginManager.h"
#include "PluginSession.h"
#include "MClientEvent.h"

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QVariant>

Q_EXPORT_PLUGIN2(simpledisplay, SimpleDisplay)

SimpleDisplay::SimpleDisplay(QObject* parent) 
        : MClientPlugin(parent) {
    _shortName = "simpledisplay";
    _longName = "Simple Display";
    _description = "A simple display plugin using QTextEdit.";
//   _dependencies.insert("terrible_test_api", 1);
    _implemented.insert("display",1);
    _receivesDataTypes << "DisplayData" << "DisplayPrompt"
		       << "UserInput" << "DoneLoading";
    //_deliversDataTypes << "?";
    _configurable = false;
    _configVersion = "2.0";
}


SimpleDisplay::~SimpleDisplay() {
}


void SimpleDisplay::configure() {
}


bool SimpleDisplay::startSession(PluginSession *ps) {
  QString s = ps->session();
  qDebug() << "* starting SimpleDisplay for session" << s;
  _eventHandlers[s] = new EventHandler(ps, this);
  return true;
}


bool SimpleDisplay::stopSession(PluginSession *ps) {
  QString s = ps->session();
  qDebug() << "* removed Simple DisplayWidget for session" << s << this;
  _eventHandlers[ps->session()]->deleteLater();
  _eventHandlers.remove(ps->session());
  return true;
}


MClientEventHandler* SimpleDisplay::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}
