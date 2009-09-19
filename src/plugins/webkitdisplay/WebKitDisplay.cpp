#include "WebKitDisplay.h"

#include "PluginSession.h"

#include "EventHandler.h"
#include "MClientEvent.h"

#include <QDebug>
#include <QEvent>
#include <QVariant>

Q_EXPORT_PLUGIN2(webkitdisplay, WebKitDisplay)

WebKitDisplay::WebKitDisplay(QWidget* parent) 
        : MClientPlugin(parent) {
    _shortName = "webkitdisplay";
    _longName = "WebKit Display";
    _description = "A display plugin using WebKit.";
//   _dependencies.insert("terrible_test_api", 1);
//    _implemented.insert("some_other_api",1);
    _receivesDataTypes << "DisplayData" << "UserInput" << "DoneLoading";
    //_deliversDataTypes << "?";
    _configurable = false;
}


WebKitDisplay::~WebKitDisplay() {
}


void WebKitDisplay::configure() {
}


bool WebKitDisplay::startSession(PluginSession *ps) {
  QString s = ps->session();
  qDebug() << "* starting WebKitDisplay for session" << s;
  _eventHandlers[s] = new EventHandler(ps, this);
  return true;
}


bool WebKitDisplay::stopSession(PluginSession *ps) {
  QString s = ps->session();
  qDebug() << "* removed Simple DisplayWidget for session" << s << this;
  delete _eventHandlers[s];
  return true;
}


MClientEventHandler* WebKitDisplay::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}
