#include <QDebug>
#include <QVariant>

#include "QtScriptPlugin.h"

#include "EventHandler.h"

#include "AbstractPluginSession.h"

#include "CommandProcessor.h"
#include "CommandEntry.h"

Q_EXPORT_PLUGIN2(qtscriptplugin, QtScriptPlugin)

QtScriptPlugin::QtScriptPlugin(QObject *parent) 
        : MClientPlugin(parent) {
    _shortName = "qtscriptplugin";
    _longName = "QtScripting Plugin";
    _description = "A JavaScript scripting language";
    //_dependencies.insert("terrible_test_api", 1);
    //_implemented.insert("some_other_api",1);
    _receivesDataTypes.insert("QtScriptEvaluate", 3);
    _receivesDataTypes.insert("QtScriptVariable", 3);
    //_deliversDataTypes << "DisplayData";
    _configurable = false;
    _configVersion = "2.0";

    // Command: script
    CommandEntry *script = new CommandEntry();
    script->pluginName(shortName());
    script->command("script");
    script->help("evaluate a script");
    script->dataType("QtScriptEvaluate");
    script->locking(true);

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


bool QtScriptPlugin::startSession(AbstractPluginSession *ps) {
  _eventHandlers[ps->session()] = new EventHandler(ps, this);
  return true;
}


bool QtScriptPlugin::stopSession(const QString &session) {
  _eventHandlers[session]->deleteLater();
  _eventHandlers.remove(session);
  return true;
}


MClientEventHandler* QtScriptPlugin::getEventHandler(const QString &s) {
  return _eventHandlers[s].data();
}
