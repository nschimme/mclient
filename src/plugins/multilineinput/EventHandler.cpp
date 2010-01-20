#include "EventHandler.h"

#include <QEvent>
#include <QVariant>
#include <QCoreApplication> // for sending command

#include "MClientEvent.h"

#include "PluginSession.h"
#include "CommandProcessor.h"

#include "StackedWidget.h"

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
    else if (me->dataTypes().contains("AddTabHistory")) {
      emit addTabHistory(me->payload()->toStringList());

    }

  }
}


QWidget* EventHandler::createWidget() {
  _widget = new StackedWidget(this);

  return _widget;
}



void EventHandler::sendUserInput(const QString &input,
				 bool echo) {
  if (echo) {
    QVariant *qv = new QVariant(input + "\r\n");
    QStringList sl("UserInput");
    postSession(qv, sl);
  }
  
  /*
  // Post to command processor
  QVariant *payload = new QVariant(input);
  QStringList tags("UserInput");
  MClientEventData *med = new MClientEventData(payload, tags,
					       _pluginSession->session());
  MClientEvent* me = new MClientEvent(med);
  QCoreApplication::postEvent(_pluginSession->getCommand()->getUserInput(),
			      me);
  */
  
  // We submit to MMapperPlugin first
  QVariant *qv = new QVariant(input);
  QStringList tags("MMapperInput");
  postSession(qv, tags);

}


void EventHandler::displayMessage(const QString& message) {
  QVariant* qv = new QVariant(message);
  QStringList sl("DisplayData");
  postSession(qv, sl);
}
