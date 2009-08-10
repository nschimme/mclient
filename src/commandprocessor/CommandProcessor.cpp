#include "CommandProcessor.h"

#include "PluginSession.h"
#include "CommandTask.h"
#include "CommandEntry.h"

#include <QDebug>
#include <QApplication>

CommandProcessor::CommandProcessor(PluginSession *ps, QObject* parent)
  : QObject(parent), _pluginSession(ps) {
  _delim = QChar(';');  // Command delimeter symbol
  _symbol = QChar('#'); // Command prefix symbol

  // Internal Commands have a Null datatype.
  _mapping.insert("quit", QString());
  _mapping.insert("qui", QString());
  _mapping.insert("help", QString());
  _mapping.insert("version", QString());
  _mapping.insert("emulate", QString());
  _mapping.insert("print", QString());
  _mapping.insert("delim", QString());
  _mapping.insert("beep", QString());
  _mapping.insert("alias", QString());
  _mapping.insert("action", QString());
  _mapping.insert("split", QString());

  // Start the command task threads
  _actionTask = new CommandTask(COMMAND_ACTION, this);
  _userInputTask = new CommandTask(COMMAND_ALIAS, this);
  _actionTask->start();
  _userInputTask->start();

  qDebug() << "CommandProcessor created with thread:" << this->thread();
}


CommandProcessor::~CommandProcessor() {
  this->deleteLater();
  qDebug() << "* CommandProcessor destroyed";
}


/*
void CommandProcessor::customEvent(QEvent* e) {
  if (e->type() == 10000) {
    // EngineEvent
    return ;

  }
  else if (e->type() == 10001) {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    if(me->dataTypes().contains("XMLAll")) {
      CommandTask *task = new CommandTask(me->payload()->toString(),
					  me->dataTypes(),
					  this);
      QThreadPool::globalInstance()->start(task);
    }
  }
}


void CommandProcessor::parseInput(const QString &input) {
  CommandTask *task = new CommandTask(input, this);
  QThreadPool::globalInstance()->start(task);
}
*/


QObject* CommandProcessor::getUserInput() const {
  return _userInputTask;
}


QObject* CommandProcessor::getAction() const {
  return _actionTask;
}


bool CommandProcessor::unregisterCommand(const QString &source) {
  if (!_registry.contains(source)) {
    qDebug() << source << "was not registered! Unable to remove.";
    return false;
  }

  QMultiHash<QString, QString>::iterator i = _registry.find(source);
  while (i != _registry.end() && i.key() == source) {
    if (_mapping.remove(i.value()) == 0)
      qDebug() << "Unable to remove command" << i.value()
	       << "for plugin" << source;
    ++i;
  }
  _registry.remove(source);
  return true;
}


void CommandProcessor::registerCommand(const QString &pluginName,
				       const QList<CommandEntry* > &list) {
  foreach(CommandEntry *ce, list) {
    if (_mapping.contains(ce->command())) {
      qDebug() << "Error, command" << ce->command() << "was already added";
    } else {
      qDebug() << "Registering command " << ce->command() << " "
	       << ce->dataType();
      _mapping.insert(ce->command(), ce->dataType());
      _registry.insertMulti(pluginName, ce->command());
    }
  }
}


void CommandProcessor::configure() {
}


bool CommandProcessor::loadSettings() {
    return true;
}


bool CommandProcessor::saveSettings() const {
    return true;
}


void CommandProcessor::emitQuit() {
  emit quit();
}
