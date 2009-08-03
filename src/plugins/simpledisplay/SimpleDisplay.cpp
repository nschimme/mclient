#include "SimpleDisplay.h"
#include "ClientTextEdit.h"

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
  exit();
}


void SimpleDisplay::customEvent(QEvent* e) {
  if(!e->type() == 10001) return;
  
  MClientEvent* me = static_cast<MClientEvent*>(e);
  if (me->dataTypes().contains("DisplayData")) {
    emit displayText(me->payload()->toString());

  }
  else if (me->dataTypes().contains("UserInput")) {
    emit displayText(me->payload()->toString());

  }
}


void SimpleDisplay::run() {
  exec();
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
  return true;
}


bool SimpleDisplay::stopSession(QString s) {
  if (_widget->close())
    qDebug() << "* removed Simple DisplayWidget for session" << s << this;
  return true;
}


// Display plugin members
bool SimpleDisplay::initDisplay(QString) {
  _widget = new ClientTextEdit;

  connect(this, SIGNAL(displayText(const QString&)),
	  _widget, SLOT(displayText(const QString&)));
  
  return true;
}

QWidget* SimpleDisplay::getWidget(QString) {
    return _widget;
}
