#include "CommandProcessor.h"

#include "PluginSession.h"
#include "CommandTask.h"
#include "CommandEntry.h"

#include "MClientPlugin.h"
#include "MClientEventHandler.h"

#include "MClientEvent.h"

#include <QDebug>
#include <QApplication>
#include <QPluginLoader>
#include <QSemaphore>

CommandProcessor::CommandProcessor(PluginSession *ps)
  : QThread(0), _pluginSession(ps) {
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
					    "delete/list/define aliases",
					    CMD_MULTI_LINE));
  _mapping.insert("action", new CommandEntry("action",
					     "delete/list/define actions",
					     CMD_MULTI_LINE));
  _mapping.insert("split", new CommandEntry("split",
					    "",
					    CMD_ONE_LINE));
  _mapping.insert("session", new CommandEntry("session",
					     "start or stop a session",
					     CMD_ONE_LINE));

  qDebug() << "CommandProcessor created with thread:" << this->thread();
}


CommandProcessor::~CommandProcessor() {
  this->deleteLater();
  qDebug() << "* CommandProcessor destroyed";
}


void CommandProcessor::customEvent(QEvent* e) {
  if (e->type() == 10000) {
    // EngineEvent
    return ;

  }
  else if (e->type() == 10001) {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    if(me->dataTypes().contains("XMLAll")) {
      emit parseMudOutput(me->payload()->toString(),
			  me->dataTypes());

    } else if (me->dataTypes().contains("UserInput")) {
      emit parseUserInput(me->payload()->toString());

    } else if (me->dataTypes().contains("SocketConnected")) {
      emit socketOpen(true);

    } else if (me->dataTypes().contains("SocketDisconnected")) {
      emit socketOpen(false);

    } else if (me->dataTypes().contains("UnlockProcessor")) {
      qDebug() << "* got UNLOCK message";
      _task->_semaphore.release();

    }
  }
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
				       const QList<CommandEntry*>&list) {
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


void CommandProcessor::run() {
  qDebug() << "CommandProcessor run" << QThread::currentThread();

  // Start the command task threads
  _task = new CommandTask(this);
  
  exec();
  qDebug() << "CommandProcessor" << QThread::currentThread() << "done";
}


void CommandProcessor::emitQuit() {
  emit quit();
}
