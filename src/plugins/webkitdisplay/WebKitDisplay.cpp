#include "WebKitDisplay.h"
#include "DisplayWidget.h"
#include "DisplayParser.h"

#include "PluginManager.h"
#include "PluginSession.h"
#include "MainWindow.h"
#include "MClientEvent.h"

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QVariant>
#include <QtWebKit>

Q_EXPORT_PLUGIN2(webkitdisplay, WebKitDisplay)

WebKitDisplay::WebKitDisplay(QWidget* parent) 
        : MClientDisplayPlugin(parent) {
    _shortName = "webkitdisplay";
    _longName = "WebKit Display";
    _description = "A display plugin using WebKit.";
//   _dependencies.insert("terrible_test_api", 1);
//    _implemented.insert("some_other_api",1);
    _receivesDataTypes << "DisplayData" << "UserInput";
    //_deliversDataTypes << "?";
    _configurable = false;
    _configVersion = "2.0";

    // Set up the Browser
    QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, false);

    // Debugging
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    // Allowable Display Locations
    SET(_displayLocations, DL_DISPLAY);
}


WebKitDisplay::~WebKitDisplay() {
  exit();
  qDebug() << "* WebKitDisplay thread quit";
}


void WebKitDisplay::customEvent(QEvent* e) {
  if(!e->type() == 10001) return;
  
  MClientEvent* me = static_cast<MClientEvent*>(e);
  if (me->dataTypes().contains("DisplayData")) {
    emit displayData(me->payload()->toString());

  }
  else if (me->dataTypes().contains("UserInput")) {
    emit userInput(me->payload()->toString());

  }
}


void WebKitDisplay::run() {
  _parser = new DisplayParser;

  connect(this, SIGNAL(displayData(const QString&)),
	  _parser, SLOT(displayData(const QString&)));
  connect(this, SIGNAL(userInput(const QString&)),
	  _parser, SLOT(userInput(const QString&)));

  exec();
}


void WebKitDisplay::configure() {
}


bool WebKitDisplay::loadSettings() {
 return true;
}

        
bool WebKitDisplay::saveSettings() const {
  return true;
}


bool WebKitDisplay::startSession(QString) {
  start(LowPriority);
  return true;
}


bool WebKitDisplay::stopSession(QString s) {
  if (_widget->close())
    qDebug() << "* removed WebKit DisplayWidget for session" << s;
  _parser->deleteLater();
  exit();
  return true;
}


// Display plugin members
bool WebKitDisplay::initDisplay(QString s) {
  MainWindow *mw = _pluginSession->getManager()->getMainWindow();
  _widget = new DisplayWidget(s, this, mw);

  /* I made this blocking so that it doesn't look like the program
     has locked up. The problem is that the Javascript DOM is slow. */
  connect(_parser, SIGNAL(displayText(const QString&)),
	  _widget, SLOT(appendText(const QString&)),
	  Qt::BlockingQueuedConnection);
  
  return true;
}

QWidget* WebKitDisplay::getWidget(QString) {
    return _widget;
}
