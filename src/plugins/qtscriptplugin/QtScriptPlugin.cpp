#include <QDebug>
#include <QVariant>
#include <QtScript>

#include "CommandManager.h"
#include "QtScriptPlugin.h"
#include "ScriptEngine.h"
#include "MClientEvent.h"

Q_EXPORT_PLUGIN2(qtscriptplugin, QtScriptPlugin)

QtScriptPlugin::QtScriptPlugin(QObject *parent) 
        : MClientPlugin(parent) {
    _shortName = "qtscriptplugin";
    _longName = "QtScripting Plugin";
    _description = "A JavaScript scripting language";
    //_dependencies.insert("terrible_test_api", 1);
//    _implemented.insert("some_other_api",1);
    _dataTypes << "QtScriptEvaluate" << "QtScriptVariable";
    _configurable = false;
    _configVersion = "2.0";

    // register commands
    QStringList commands;
    commands << _shortName
	     << "script" << "QtScriptEvaluate"
	     << "var" << "QtScriptVariable";
    CommandManager::instance()->registerCommand(commands);
}


QtScriptPlugin::~QtScriptPlugin() {
    stopAllSessions();
    saveSettings();
}

void QtScriptPlugin::customEvent(QEvent* e) {
    if(!e->type() == 10001) return;
    
    MClientEvent* me;
    me = static_cast<MClientEvent*>(e);

    if(me->dataTypes().contains("QtScriptEvaluate")) {
      emit evaluate(me->payload()->toString(), me->session());
      
    } else if (me->dataTypes().contains("QtScriptVariable")) {
      emit variable(me->payload()->toString(), me->session());

    }
}

void QtScriptPlugin::configure() {
}


const bool QtScriptPlugin::loadSettings() {
  return true;
}

        
const bool QtScriptPlugin::saveSettings() const {
  return true;
}


const bool QtScriptPlugin::startSession(QString s) {
  ScriptEngine *se = new ScriptEngine(s, this);
  _engines.insert(s, se);
  _runningSessions << s;
  return true;
}

const bool QtScriptPlugin::stopSession(QString s) {
  foreach(ScriptEngine *se, _engines.values(s)) {
    delete se;
    qDebug() << "* removed QtScriptPlugin for session" << s;
  }
  _engines.remove(s);
  int removed = _runningSessions.removeAll(s);
  return removed!=0?true:false;
}
