#include "EventHandler.h"

#include <QEvent>
#include <QVariant>

#include "MClientEvent.h"
#include "PluginSession.h"

#include "ClientTextEdit.h"

EventHandler::EventHandler(QWidget* parent) : MClientDisplayHandler(parent) {
  // Allowable Display Locations
  SET(_displayLocations, DL_DISPLAY);
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

QWidget* EventHandler::createWidget() {
  _widget = new ClientTextEdit;

  connect(this, SIGNAL(displayText(const QString&)),
	  _widget, SLOT(displayText(const QString&)));

  return _widget;
}
