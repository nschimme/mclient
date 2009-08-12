#include "SimpleDisplay.h"
#include "ClientTextEdit.h"
#include "EventHandler.h"

#include "PluginManager.h"
#include "PluginSession.h"
#include "MClientEvent.h"

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QVariant>

Q_EXPORT_PLUGIN2(simpledisplay, SimpleDisplay)

SimpleDisplay::SimpleDisplay(QWidget* parent) 
        : MClientDisplayPlugin(parent) {
    _shortName = "simpledisplay";
    _longName = "Simple Display";
    _description = "A simple display plugin using QTextEdit.";
//   _dependencies.insert("terrible_test_api", 1);
//    _implemented.insert("some_other_api",1);
    _receivesDataTypes << "DisplayData" << "UserInput";
    //_deliversDataTypes << "?";
    _configurable = false;
    _configVersion = "2.0";

    // Allowable Display Locations
    SET(_displayLocations, DL_DISPLAY);

    qDebug() << "### created a new instance of simple display" << this;
}


SimpleDisplay::~SimpleDisplay() {
}


void SimpleDisplay::configure() {
}


bool SimpleDisplay::loadSettings() {
 return true;
}

        
bool SimpleDisplay::saveSettings() const {
  return true;
}


bool SimpleDisplay::startSession(QString s) {
  qDebug() << "* starting SimpleDisplay for session" << s;
  _eventHandlers[s] = new EventHandler;
  return true;
}


bool SimpleDisplay::stopSession(QString s) {
  if (_widgets[s]->close()) {
    qDebug() << "* removed Simple DisplayWidget for session" << s << this;
    delete _eventHandlers[s];
  }
  return true;
}


MClientEventHandler* SimpleDisplay::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}


// Display plugin members
bool SimpleDisplay::initDisplay(QString s) {
  _widgets[s] = new ClientTextEdit;

  connect(_eventHandlers[s], SIGNAL(displayText(const QString&)),
	  _widgets[s], SLOT(displayText(const QString&)));
  
  return true;
}

QWidget* SimpleDisplay::getWidget(QString s) {
  return _widgets[s];
}
