#include "EventHandler.h"

#include <QEvent>
#include <QVariant>
#include <QDebug>
#include <QCoreApplication> // for sending command

#include "MClientEvent.h"
#include "AbstractPluginSession.h"
#include "CommandProcessor.h"
#include "ConfigEntry.h"
#include "ScriptEngine.h"

EventHandler::EventHandler(AbstractPluginSession *ps, MClientPlugin *mp)
  : MClientEventHandler(ps, mp) {

  _scriptEngine = new ScriptEngine(this);

  // Connect Signals/Slots
  connect(this, SIGNAL(evaluate(const QString&)),
	  _scriptEngine, SLOT(evaluateExpression(const QString&)));
  connect(this, SIGNAL(variable(const QString&)),
	  _scriptEngine, SLOT(variableCommand(const QString&)));
  connect(_scriptEngine, SIGNAL(emitParseInput(const QString&)),
	  this, SLOT(parseInput(const QString&)));
  connect(_scriptEngine, SIGNAL(postSession(QVariant*, const QStringList&)),
	  this, SLOT(postSession(QVariant*, const QStringList&)));
}


EventHandler::~EventHandler() {
  qDebug() << "* removed QtScriptPlugin for session"
	   << _pluginSession->session();
}


void EventHandler::customEvent(QEvent *e) {
  if (e->type() == 10001) {

    // Forward the event to the next in the chain
    forwardEvent(e);

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
  QCoreApplication::postEvent(_pluginSession->getCommand(), me); 
}
