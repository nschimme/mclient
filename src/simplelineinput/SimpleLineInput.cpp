#include <QApplication>
#include <QDebug>
#include <QVariant>

#include "SimpleLineInput.h"
#include "InputWidget.h"

#include "MClientEvent.h"
#include "MClientEventData.h"
#include "PluginManager.h"
#include "CommandManager.h"

Q_EXPORT_PLUGIN2(simplelineinput, SimpleLineInput)


SimpleLineInput::SimpleLineInput(QWidget* parent) 
        : MClientIOPlugin(parent) {
    _shortName = "simplelineinput";
    _longName = "Simple Line Input";
    _description = "A simple line input plugin.";
    //_dependencies.insert("terrible_test_api", 1);
//    _implemented.insert("some_other_api",1);
    _dataTypes << "ChangeUserInput";
    _configurable = false;
    _configVersion = "2.0";

    // SocketManager members
    _settingsFile = "config/"+_shortName+".xml";

    // Allowable Display Locations
    SET(_displayLocations, DL_FLOAT);
    SET(_displayLocations, DL_BOTTOM);
}


SimpleLineInput::~SimpleLineInput() {
    stopAllSessions();
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

void SimpleLineInput::sendUserInput(const QString &session, const QString &input) {
  CommandManager::instance()->parseInput(input, session);
}

void SimpleLineInput::configure() {
}


const bool SimpleLineInput::loadSettings() {
  return true;
}

        
const bool SimpleLineInput::saveSettings() const {
  return true;
}


const bool SimpleLineInput::startSession(QString s) {
    initDisplay(s);
    _runningSessions << s;
    return true;
}


const bool SimpleLineInput::stopSession(QString s) {
  foreach(InputWidget *iw, _widgets.values(s)) {
    if (iw->close())
      qDebug() << "* removed SimpleLineInput InputWidget for session" << s;
  }
  _widgets.remove(s);
  int removed = _runningSessions.removeAll(s);
  return removed!=0?true:false;
}


// Display plugin members
const bool SimpleLineInput::initDisplay(QString s) {
    InputWidget* widget = new InputWidget(s, this);
    _widgets.insert(s, widget); 
    widget->show();
    return true;
}

const QWidget* SimpleLineInput::getWidget(QString s) {
    return (QWidget*)_widgets[s];
}
