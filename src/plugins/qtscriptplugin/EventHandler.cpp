#include "EventHandler.h"

#include <QEvent>
#include <QVariant>
#include <QDebug>
#include <QCoreApplication> // for sending command

#include "MClientEvent.h"

#include "PluginSession.h"
#include "CommandProcessor.h"

EventHandler::EventHandler(PluginSession *ps, MClientPlugin *mp)
  : MClientEventHandler(ps, mp) {
}


EventHandler::~EventHandler() {
  qDebug() << "* removed QtScriptPlugin for session"
	   << _pluginSession->session();
}


void EventHandler::customEvent(QEvent *e) {
  if (e->type() == 10000)
    engineEvent(e);

  else {   
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    if(me->dataTypes().contains("QtScriptEvaluate")) {
      emit evaluate(me->payload()->toString());
      
    } else if (me->dataTypes().contains("QtScriptVariable")) {
      emit variable(me->payload()->toString());

    }
  }
}


void EventHandler::parseInput(const QString &input) {
  // Post to command processor
  QVariant *payload = new QVariant(input);
  QStringList tags("ScriptResult");
  MClientEventData *med = new MClientEventData(payload, tags,
					       _pluginSession->session());
  MClientEvent* me = new MClientEvent(med);
  QCoreApplication::postEvent(_pluginSession->getCommand()->getUserInput(),
			      me); 
}
