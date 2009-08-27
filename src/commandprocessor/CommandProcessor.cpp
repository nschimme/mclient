#include "CommandProcessor.h"

#include "PluginSession.h"
#include "UserCommandTask.h"
#include "MudCommandTask.h"
#include "CommandEntry.h"

#include <QDebug>
#include <QApplication>

CommandProcessor::CommandProcessor(PluginSession *ps, QObject* parent)
  : QObject(parent), _pluginSession(ps) {
  _delim = QChar(';');  // Command delimeter symbol
  _symbol = QChar('#'); // Command prefix symbol

  // Internal Commands have a no plugin name.
  _mapping.insert("quit", new CommandEntry("quit",
					   "quits the program",
					   CMD_ONE_LINE));
  _mapping.insert("qui", new CommandEntry("qui",
					  "",
					  CMD_ONE_LINE));
  _mapping.insert("help", new CommandEntry("help",
					   "displays this information",
					   CMD_ONE_LINE));
  _mapping.insert("version", new CommandEntry("version",
					      "version information",
					      CMD_ONE_LINE));
  _mapping.insert("emulate", new CommandEntry("emulate",
					      "",
					      CMD_ONE_LINE));
  _mapping.insert("print", new CommandEntry("print",
					    "",
					    CMD_ONE_LINE));
  _mapping.insert("delim", new CommandEntry("delim",
					    "",
					    CMD_ONE_LINE));
  _mapping.insert("beep", new CommandEntry("beep",
					   "",
					   CMD_ONE_LINE));
  _mapping.insert("alias", new CommandEntry("alias",
					    "",
					    CMD_ONE_LINE));
  _mapping.insert("action", new CommandEntry("action",
					     "",
					     CMD_ONE_LINE));
  _mapping.insert("split", new CommandEntry("split",
					    "",
					    CMD_ONE_LINE));

  // Start the command task threads
  _actionTask = new MudCommandTask(this);
  _userInputTask = new UserCommandTask(this);
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
  int removed = 0;
  CommandMapping::iterator i = _mapping.begin();
  while (i != _mapping.end() && i.value()->pluginName() == source) {
    if (_mapping.remove(i.key()) == 0)
      qDebug() << "Unable to remove command" << i.value()
	       << "for plugin" << source;
    ++i;
    removed++;
  }

  if (removed == 0) {
    qDebug() << source << "was not registered! Unable to remove.";
    return false;
  }

  return true;
}


void CommandProcessor::registerCommand(const QString &pluginName,
				       const QList<CommandEntry* > &list) {
  foreach(CommandEntry *ce, list) {
    if (_mapping.contains(ce->command())) {
      qDebug() << "Error, command" << ce->command() << "was already added";
    } else {
      qDebug() << "Registering command " << ce->command() << " "
	       << ce->dataType() << "for" << pluginName;
      _mapping.insert(ce->command(), ce);
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
