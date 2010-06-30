#include "EventHandler.h"

#include <QEvent>
#include <QVariant>
#include <QCoreApplication> // for sending command

#include "MClientEvent.h"

#include "PluginSession.h"
#include "CommandProcessor.h"

#include "InputWidget.h"

EventHandler::EventHandler(PluginSession *ps, MClientPlugin *mp)
  : MClientDisplayHandler(ps, mp) {
  // Allowable Display Locations
  SET(_displayLocations, DL_INPUT);
}


EventHandler::~EventHandler() {
}


void EventHandler::customEvent(QEvent *e) {
  if (e->type() == 10000)
    engineEvent(e);

  else if (e->type() == 10001) {

    // Forward the event to the next in the chain
    forwardEvent(e);

    MClientEvent *me = static_cast<MClientEvent*>(e);
    
    if(me->dataTypes().contains("CommandHistory")) {
      // Display command history
      emit showCommandHistory();

    }
    else if (me->dataTypes().contains("EchoMode")) {
      emit setEchoMode(me->payload()->toBool());

    }
    else if (me->dataTypes().contains("SocketDisconnected")) {
      emit setEchoMode(true);

    }

  }
}


QWidget* EventHandler::createWidget() {
  _widget = new InputWidget;  

  // Connect Signals/Slots
  connect(_widget, SIGNAL(sendUserInput(const QString&, bool)),
	  SLOT(sendUserInput(const QString&, bool)));
  connect(_widget, SIGNAL(displayMessage(const QString &)),
	  SLOT(displayMessage(const QString &)));
  connect(this, SIGNAL(setEchoMode(bool)),
	  _widget, SLOT(toggleEchoMode(bool)));

  return _widget;
}



void EventHandler::sendUserInput(const QString &input,
					       bool echo) {
  if (echo) {
    QVariant *qv = new QVariant(input + "\n");
    QStringList sl("UserInput");
    postSession(qv, sl);
  }

  QVariant *qv = new QVariant(input);
  QStringList tags("MMapperInput");
  postSession(qv, tags);
}


void EventHandler::displayMessage(const QString& message) {
  QVariant* qv = new QVariant(message);
  QStringList sl("DisplayData");
  postSession(qv, sl);
}
