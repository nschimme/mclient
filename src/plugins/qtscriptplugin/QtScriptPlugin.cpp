#include <QDebug>
#include <QVariant>
#include <QtScript>

#include "PluginManager.h"
#include "PluginSession.h"
#include "CommandProcessor.h"
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
    _receivesDataTypes << "QtScriptEvaluate" << "QtScriptVariable";
    _deliversDataTypes << "DisplayData";
    _configurable = false;
    _configVersion = "2.0";
}


QtScriptPlugin::~QtScriptPlugin() {
    saveSettings();
    exit();
}

void QtScriptPlugin::customEvent(QEvent* e) {
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

void QtScriptPlugin::run() {
  _engine = new ScriptEngine(_session, this);
  exec();
}

void QtScriptPlugin::configure() {
}


bool QtScriptPlugin::loadSettings() {
  // register commands
  QStringList commands;
  commands << _shortName
	   << "script" << "QtScriptEvaluate"
	   << "var" << "QtScriptVariable";
  _pluginSession->getCommand()->registerCommand(commands);

  return true;
}

        
bool QtScriptPlugin::saveSettings() const {
  return true;
}


bool QtScriptPlugin::startSession(QString /* s */) {
  if (!isRunning()) start(LowPriority);
  return true;
}

bool QtScriptPlugin::stopSession(QString s) {
  _engine->deleteLater();
  exit();
  qDebug() << "* removed QtScriptPlugin for session" << s;
  return true;
}

void QtScriptPlugin::parseInput(const QString &input) {
  _pluginSession->getCommand()->parseInput(input);
}
