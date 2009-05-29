#include "CommandManager.h"

#include "MClientEvent.h"
#include "PluginManager.h"

#include <QDebug>
#include <QStringList>
#include <QVariant>

CommandManager* CommandManager::_pinstance = 0;

CommandManager* CommandManager::instance() {
    if(!_pinstance) {
        _pinstance = new CommandManager();
    }
    
    return _pinstance;
}


void CommandManager::destroy() {
    delete this;
}

CommandManager::CommandManager(QObject* parent) : QObject(parent) {
  // Command prefix symbol
  _symbol = QChar('#');
}


CommandManager::~CommandManager() {
}

bool CommandManager::unregisterCommand(const Source& source) {
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


void CommandManager::registerCommand(const QStringList& sl) {
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
      _registry.insert(source, command);
    }
  }
}


bool CommandManager::parseInput(const QString& input,
				const QString& session) {
  // Display Data
  QByteArray ba(input.toAscii());
  ba.append("\n");
  QVariant* qv = new QVariant(ba);
  QStringList sl("XMLDisplayData");
  postEvent(qv, sl, session);
  
  if (!input.startsWith(_symbol)) {
      // Send to Socket
      sl.clear();
      sl << "SendToSocketData";
      postEvent(qv, sl, session);
      return true;

  } else {
    // Parse as a Plugin's Command
    Command command(input);
    QString arguments;
    QRegExp whitespace("\\s+");

    int whitespaceIndex = input.indexOf(whitespace);
    if (whitespaceIndex >= 0) {
      command = input.mid(1, whitespaceIndex-1);
      arguments = input.mid(whitespaceIndex+1);
    } else {
      command = input.mid(1);
    }

    qDebug() << "* CommandManager got an event: " << command << arguments;
  
    if (_mapping.contains(command)) {
      // Relay command to corresponding plugin
      QVariant* qv = new QVariant(arguments);
      QStringList sl;
      sl << _mapping.value(command);
      postEvent(qv, sl, session);
      return true;
      
    } else {
      // Unknown command!
      QString errorString = "#unknown command \"" + command + "\"\n";
      QVariant* qv = new QVariant(errorString);
      QStringList sl;
      sl << "XMLDisplayData";
      postEvent(qv, sl, session);
    }
  }
  return false;
}

void CommandManager::configure() {
}


const bool CommandManager::loadSettings() {
    return true;
}


const bool CommandManager::saveSettings() const {
    return true;
}


void CommandManager::postEvent(QVariant *payload, const QStringList& tags, 
        const QString& session) {
    PluginManager* pm = PluginManager::instance();
    MClientEvent* me = new MClientEvent(new MClientEventData(payload), tags,
					session);
    pm->customEvent(me);
}
