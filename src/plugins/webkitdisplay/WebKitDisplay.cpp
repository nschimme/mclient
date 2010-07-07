#include "WebKitDisplay.h"

#include "AbstractPluginSession.h"

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
    _receivesDataTypes.insert("DisplayData", 3);
    _receivesDataTypes.insert("DisplayPrompt", 3);
    _receivesDataTypes.insert("UserInput", 3);
    _receivesDataTypes.insert("DoneLoading", 3);
    //_deliversDataTypes << "?";
    _configurable = false;
}


WebKitDisplay::~WebKitDisplay() {
  qDebug() << "~WebKitDisplay";
}


void WebKitDisplay::configure() {
}


bool WebKitDisplay::startSession(AbstractPluginSession *ps) {
  const QString &s = ps->session();
  qDebug() << "* starting WebKitDisplay for session" << s;
  _eventHandlers[s] = new EventHandler(ps, this);
  return true;
}


bool WebKitDisplay::stopSession(const QString &session) {
  qDebug() << "* removed Simple DisplayWidget for session" << session << this;
  _eventHandlers[session]->deleteLater();
  _eventHandlers.remove(session);
  return true;
}


MClientEventHandler* WebKitDisplay::getEventHandler(const QString &session) {
  return _eventHandlers[session].data();
}
