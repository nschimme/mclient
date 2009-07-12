#include <QApplication>
#include <QDebug>
#include <QVariant>

#include "MultiLineInput.h"
#include "InputWidget.h"

#include "PluginManager.h"
#include "PluginSession.h"
//#include "MainWindow.h"
#include "CommandProcessor.h"
#include "MClientEvent.h"

Q_EXPORT_PLUGIN2(multilineinput, MultiLineInput)


MultiLineInput::MultiLineInput(QWidget* parent) 
        : MClientDisplayPlugin(parent) {
    _shortName = "multilineinput";
    _longName = "Multi Line Input";
    _description = "A multi lined input plugin.";
    //_dependencies.insert("terrible_test_api", 1);
//    _implemented.insert("some_other_api",1);
    _receivesDataTypes << "ChangeUserInput" << "EchoMode"
		       << "SocketDisconnected";
    _deliversDataTypes << "UserInput";
    _configurable = false;
    _configVersion = "2.0";

    // SocketManager members
    _settingsFile = "config/"+_shortName+".xml";

    // Allowable Display Locations
    SET(_displayLocations, DL_INPUT);
}


MultiLineInput::~MultiLineInput() {
    saveSettings();
}

void MultiLineInput::customEvent(QEvent* e) {
  if (e->type() == 10000)
    engineEvent(e);
  else if (e->type() == 10001) {
    MClientEvent *me = static_cast<MClientEvent*>(e);
    
    if(me->dataTypes().contains("ChangeUserInput")) {
      // TODO: History, tab-completion

    }
    else if (me->dataTypes().contains("EchoMode")) {
      emit setEchoMode(me->payload()->toBool());

    }
    else if (me->dataTypes().contains("SocketDisconnected")) {
      emit setEchoMode(true);

    }

  }
}

void MultiLineInput::sendUserInput(const QString &input, bool echo) {
  if (echo) {
    QVariant* qv = new QVariant(input + "\n");
    QStringList sl("UserInput");
    postSession(qv, sl);
  }

  _pluginSession->getCommand()->parseInput(input);
}

void MultiLineInput::configure() {
}


bool MultiLineInput::loadSettings() {

  return true;
}

        
bool MultiLineInput::saveSettings() const {
  return true;
}


bool MultiLineInput::startSession(QString) {
    return true;
}


bool MultiLineInput::stopSession(QString s) {
  if (_widget->close())
    qDebug() << "* removed MultiLineInput InputWidget for session" << s;
  return true;
}


// Display plugin members
bool MultiLineInput::initDisplay(QString s) {
  //MainWindow *mw = _pluginSession->getManager()->getMainWindow();
  _widget = new InputWidget(s, this);
  
  return true;
}

QWidget* MultiLineInput::getWidget(QString) {
    return _widget;
}
