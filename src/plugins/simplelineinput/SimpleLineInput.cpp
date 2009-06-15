#include <QApplication>
#include <QDebug>
#include <QVariant>

#include "SimpleLineInput.h"
#include "InputWidget.h"

#include "PluginManager.h"
#include "PluginSession.h"
#include "MainWindow.h"
#include "CommandManager.h"
#include "MClientEvent.h"

Q_EXPORT_PLUGIN2(simplelineinput, SimpleLineInput)


SimpleLineInput::SimpleLineInput(QWidget* parent) 
        : MClientDisplayPlugin(parent) {
    _shortName = "simplelineinput";
    _longName = "Simple Line Input";
    _description = "A simple line input plugin.";
    //_dependencies.insert("terrible_test_api", 1);
//    _implemented.insert("some_other_api",1);
    _receivesDataTypes << "ChangeUserInput";
    //    _deliversDataTypes << "?";
    _configurable = false;
    _configVersion = "2.0";

    // SocketManager members
    _settingsFile = "config/"+_shortName+".xml";

    // Allowable Display Locations
    SET(_displayLocations, DL_INPUT);
}


SimpleLineInput::~SimpleLineInput() {
    saveSettings();
}

void SimpleLineInput::customEvent(QEvent* e) {
//    qDebug() << "* bork bork bork!";
    if(!e->type() == 10001) return;
    
    MClientEvent* me;
    me = static_cast<MClientEvent*>(e);

    if(me->dataTypes().contains("ChangeUserInput")) {
      //        QByteArray ba = me->payload()->toByteArray();
//        qDebug() << "Displayed: " << ba.data();
//        QVariant* qv = new QVariant(ba);
      //emit dataReceived(qv->toByteArray());
    }
}

void SimpleLineInput::sendUserInput(const QString &input) {
  _pluginSession->getManager()->getCommand()->parseInput(input, _session);
}

void SimpleLineInput::configure() {
}


bool SimpleLineInput::loadSettings() {
  return true;
}

        
bool SimpleLineInput::saveSettings() const {
  return true;
}


bool SimpleLineInput::startSession(QString s) {
    initDisplay(s);
    return true;
}


bool SimpleLineInput::stopSession(QString s) {
  if (_widget->close())
    qDebug() << "* removed SimpleLineInput InputWidget for session" << s;
  return true;
}


// Display plugin members
bool SimpleLineInput::initDisplay(QString s) {
  MainWindow *mw = _pluginSession->getManager()->getMainWindow();
  _widget = new InputWidget(s, this, mw);
  
  return true;
}

QWidget* SimpleLineInput::getWidget(QString) {
    return _widget;
}
