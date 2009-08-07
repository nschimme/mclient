#include "EventHandler.h"

#include <QEvent>
#include <QVariant>

#include "MClientEvent.h"

#include "PluginSession.h"
#include "CommandProcessor.h"

EventHandler::EventHandler(QObject* parent) : MClientEventHandler(parent) {
}


EventHandler::~EventHandler() {
}


void EventHandler::customEvent(QEvent *e) {
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


void EventHandler::sendUserInput(const QString &input,
					       bool echo) {
  if (echo) {
    QVariant* qv = new QVariant(input + "\n");
    QStringList sl("UserInput");
    postSession(qv, sl);
  }

  _pluginSession->getCommand()->parseInput(input);
}
