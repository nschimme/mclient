#include "CommandManager.h"

#include "MClientEvent.h"
#include "PluginManager.h"

#include <QDebug>
#include <QStringList>
#include <QVariant>
#include <QApplication>

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

CommandManager::CommandManager(QObject* parent) : QThread(parent) {
  _symbol = QChar('#'); // Command prefix symbol
  _delim = QChar(';');  // Command delimeter

  // Internal Commands have a Null datatype.
  _mapping.insert("quit", QString());
  _mapping.insert("qui", QString());
  _mapping.insert("help", QString());
  _mapping.insert("version", QString());
  _mapping.insert("emulate", QString());
  _mapping.insert("delim", QString());
  _mapping.insert("beep", QString());
}


CommandManager::~CommandManager() {
}

void CommandManager::run() {
  qDebug() << "CommandManager running in thread:" << this->thread();
  exec();
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


void CommandManager::parseInput(const QString &input,
				const QString &session) {
  // Display Data
  QVariant* qv = new QVariant(input + "\n");
  QStringList sl("XMLDisplayData");
  if (input.isEmpty()) sl << "SendToSocketData";
  postEvent(qv, sl, session);

  QStringList tokens = input.split(_delim, QString::SkipEmptyParts);
  for (int i = 0; i < tokens.size(); ++i) {
    QString input = tokens.at(i);
  
    if (!input.startsWith(_symbol)) {
      // Non-command; check if it is an alias
      qv = new QVariant(input);
      sl.clear();
      sl << "AliasInput";
      postEvent(qv, sl, session);

    } else {
      // Parse as a Plugin's Command
      Command command;
      QString arguments;
      QRegExp whitespace("\\s+");
      int whitespaceIndex = input.indexOf(whitespace);
      if (whitespaceIndex >= 0) {
	command = input.mid(1, whitespaceIndex-1);
	arguments = input.mid(whitespaceIndex+1).trimmed();
      } else {
	command = input.mid(1);
      }
    
      qDebug() << "* CommandManager got an event: " << command << arguments << ".";
      
      parseCommand(command, arguments, session);
    }
  }
}

bool CommandManager::parseCommand(QString command,
				  const QString &arguments,
				  const QString &session) {
  if (command.isEmpty()) command = "help";
    
  // Identify Command
  QMapIterator<Command, DataType> i(_mapping);
  while (i.hasNext()) {
    if (i.next().key().startsWith(command)) {
      // Command was identified
      command = i.key();
      
      if (!i.value().isNull()) {
	// External commands

	// Relay command to corresponding plugin
	QVariant* qv = new QVariant(arguments);
	QStringList sl;
	sl << _mapping.value(command);
	postEvent(qv, sl, session);

      } else {
	// Internal Commands
	if (command == "emulate") {
	  QVariant* qv = new QVariant(arguments);
	  QStringList sl;
	  sl << "TelnetData";
	  postEvent(qv, sl, session);
	  
	} else if (command == "delim") {
	  if (arguments.size() == 1 && arguments.at(0) != ' ') {
	    _delim = arguments.at(0);
	    displayData("#delimeter is now " + arguments + "\n", session);
	  } else
	    displayData("#not allowed\n", session);

	} else if (command == "qui") {
	  displayData("#you have to write '#quit' - no less, to quit!\n",
		      session);
	  
	} else if (command == "quit") {
	  emit quit();

	} else if (command == "version") {
	  displayData("mClient Version ???, \251 Jahara\n",
		      session);
	  
	} else if (command == "help") {
	  QString output = "#commands available:\n";
	  QMapIterator<Command, DataType> i(_mapping);
	  while (i.hasNext())
	    output += QString("#%1\n").arg(i.next().key());
	  displayData(output, session);

	} else if (command == "beep") {
	  QApplication::beep();

	}
      }
      // Command run
      return true;
    }
  }

  // Check if it is a repeat
  bool ok;
  int repeat = command.toInt(&ok);
  if (ok) {
    if (repeat <= 500) {
      for (int i = 0; i < repeat; i++) {
	parseInput(arguments, session);
      }
    } else {
      displayData("#command ignored; too many repeats.", session);
    }
    return true;
  }

  // Unknown command!
  displayData(QString("#unknown command \"" + command + "\"\n"), session);
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

void CommandManager::displayData(const QString &output,
				 const QString &session) {
  QVariant* qv = new QVariant(output);
  QStringList sl;
  sl << "XMLDisplayData";
  postEvent(qv, sl, session);  
}

void CommandManager::postEvent(QVariant *payload, const QStringList& tags, 
        const QString& session) {
    MClientEvent *me = new MClientEvent(new MClientEventData(payload),
					tags,
					session);
    QApplication::postEvent(PluginManager::instance(),
			    me);
    //PluginManager::instance()->customEvent(me);
}
