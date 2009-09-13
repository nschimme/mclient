#include <QDebug>
#include <QVariant>

#include "QtScriptPlugin.h"

#include "EventHandler.h"

#include "PluginSession.h"
#include "CommandEntry.h"
#include "ScriptEngine.h"

Q_EXPORT_PLUGIN2(qtscriptplugin, QtScriptPlugin)

QtScriptPlugin::QtScriptPlugin(QObject *parent) 
        : MClientPlugin(parent) {
    _shortName = "qtscriptplugin";
    _longName = "QtScripting Plugin";
    _description = "A JavaScript scripting language";
    //_dependencies.insert("terrible_test_api", 1);
//    _implemented.insert("some_other_api",1);
    _receivesDataTypes << "QtScriptEvaluate" << "QtScriptVariable";
    _deliversDataTypes << "DisplayData";
    _configurable = false;
    _configVersion = "2.0";

    // Command: script
    CommandEntry *script = new CommandEntry();
    script->pluginName(shortName());
    script->command("script");
    script->help("evaluate a script");
    script->dataType("QtScriptEvaluate");

    // Command: var
    CommandEntry *var = new CommandEntry();
    var->pluginName(shortName());
    var->command("var");
    var->help("list or set a variable");
    var->dataType("QtScriptVariable");

    // For registering commands
    _commandEntries << script << var;

}


QtScriptPlugin::~QtScriptPlugin() {
}

void QtScriptPlugin::configure() {
}


bool QtScriptPlugin::startSession(PluginSession *ps) {
  QString s = ps->session();
  _eventHandlers[s] = new EventHandler(ps, this);
  _scriptEngines[s] = new ScriptEngine;

  // Connect Signals/Slots
  connect(_eventHandlers[s], SIGNAL(evaluate(const QString&)),
	  _scriptEngines[s], SLOT(evaluateExpression(const QString&)));
  connect(_eventHandlers[s], SIGNAL(variable(const QString&)),
	  _scriptEngines[s], SLOT(variableCommand(const QString&)));
  connect(_scriptEngines[s], SIGNAL(signalHandlerException(const QScriptValue&)),
	  _scriptEngines[s], SLOT(handleException(const QScriptValue&)));
  connect(_scriptEngines[s], SIGNAL(emitParseInput(const QString&)),
	  _eventHandlers[s], SLOT(parseInput(const QString&)));
  connect(_scriptEngines[s], SIGNAL(postSession(QVariant*, const QStringList&)),
	  _eventHandlers[s], SLOT(postSession(QVariant*, const QStringList&)));

  return true;
}


bool QtScriptPlugin::stopSession(PluginSession *ps) {
  QString s = ps->session();
  _eventHandlers[s]->deleteLater();
  _scriptEngines[s]->deleteLater();
  return true;
}


MClientEventHandler* QtScriptPlugin::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}
