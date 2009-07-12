#include "CommandProcessor.h"

#include "MClientEvent.h"
#include "PluginSession.h"
#include "CommandTask.h"

#include <QDebug>
#include <QApplication>

#include <QThreadPool>

CommandProcessor::CommandProcessor(PluginSession *ps, QObject* parent)
  : QThread(parent), _pluginSession(ps) {
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

  qDebug() << "CommandProcessor created with thread:" << this->thread();
}


CommandProcessor::~CommandProcessor() {
  this->deleteLater();
  qDebug() << "* CommandProcessor destroyed";
}

void CommandProcessor::run() {
  qDebug() << "CommandProcessor running in thread:" << this->thread();
  exec();
}


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

bool CommandProcessor::unregisterCommand(const Source& source) {
  if (!_registry.contains(source)) {
    qDebug() << source << "was not registered! Unable to remove.";
    return false;
  }

  QMultiHash<Source, Command>::iterator i = _registry.find(source);
  while (i != _registry.end() && i.key() == source) {
    if (_mapping.remove(i.value()) == 0)
      qDebug() << "Unable to remove command" << i.value()
	       << "for plugin" << source;
    ++i;
  }
  _registry.remove(source);
  return true;
}


void CommandProcessor::registerCommand(const QStringList& sl) {
  // First Element is Registering Source
  Source source = sl.at(0);

  // All Other Elements should be in pairs (command, dataType)
  // TODO: Rewrite this as a QHash rather than a QStringList
  for (int i = 2; i < sl.size(); i += 2) {
    Command command = sl.at(i-1);
    DataType dataType = sl.at(i);
    if (_mapping.contains(command)) {
      qDebug() << "Error, command" << command << "was already added";
    } else {
      qDebug() << "Registering command " << command << " " << dataType;
      _mapping.insert(command, dataType);
      _registry.insertMulti(source, command);
    }
  }
}

void CommandProcessor::parseInput(const QString &input) {
  CommandTask *task = new CommandTask(input, this);
  QThreadPool::globalInstance()->start(task);
}


void CommandProcessor::configure() {
}


bool CommandProcessor::loadSettings() {
    return true;
}


bool CommandProcessor::saveSettings() const {
    return true;
}