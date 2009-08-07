#include "EventHandler.h"

#include <QEvent>
#include <QVariant>

#include "MClientEvent.h"
#include "PluginSession.h"

EventHandler::EventHandler(QObject* parent) : MClientEventHandler(parent) {
}


EventHandler::~EventHandler() {
}


void EventHandler::customEvent(QEvent *e) {
  if(!e->type() == 10001) return;
  
  MClientEvent* me = static_cast<MClientEvent*>(e);
  if (me->dataTypes().contains("DisplayData")) {
    emit displayText(me->payload()->toString());

  }
  else if (me->dataTypes().contains("UserInput")) {
    emit displayText(me->payload()->toString());

  }
}
