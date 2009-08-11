#include "CommandTask.h"

#include "PluginSession.h"
#include "CommandProcessor.h"
#include "AliasManager.h"
#include "ActionManager.h"

#include "MClientEvent.h"

#include <QApplication>
#include <QDebug>
#include <QVariant>

CommandTask::CommandTask(const CommandProcessorType &type,
			 CommandProcessor *ps,
			 QObject *parent)
  : QThread(parent), _type(type), _commandProcessor(ps) {

  _queue = QStringList();
  _stack = 0;
}


CommandTask::~CommandTask() {
}


void CommandTask::customEvent(QEvent *e) {
  if (e->type() == 10000) {
    // EngineEvent
    return ;

  }
  else if (e->type() == 10001) {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    if(me->dataTypes().contains("XMLAll")) {
      findAction(me->payload()->toString(),
		 me->dataTypes());
      processStack();

    }
    else if (me->dataTypes().contains("UserInput")) {
      parseInput(me->payload()->toString());
      processStack();
     
    }
  }
  else {
    qDebug() << "CommandTask" << QThread::currentThread()
	     << "received unknown event type:" << e->type();
  }
}


void CommandTask::run() {
  qDebug() << "CommandTask run" << QThread::currentThread();
  exec();
  qDebug() << "CommandTask" << QThread::currentThread() << "done";
}


void CommandTask::parseInput(const QString &input, const QChar &splitChar) {
  // Add current input to the top of the stack
  QListIterator<QString> i(input.split(splitChar); // QString::SkipEmptyParts
  for (i.toBack(); i.hasPrevious();) {
    _queue.append(i.previous());
    qDebug() << "* adding" << _queue.last() << "to stack";
  }
}

void CommandTask::processStack() {
  while (!_queue.isEmpty()) {
    qDebug() << "* Command queue:" << _stack << _queue;

    if (_stack++ > 100) {
      _queue.clear();
      qWarning() << "! Stack overflow!";
      displayData("#error: stack overflow.\n");
      return ;
    }

    // Pop stack
    QString current = _queue.takeLast();
    qDebug() << "* popping" << current << "from stack";    

    QString command, arguments;
    
    QRegExp rx("^\\s*(\\S+)");
    int rxIndex = rx.indexIn(current);
    if (rxIndex >= 0) {
      command = rx.cap(1);
      arguments = current.mid(rx.matchedLength()+1);
    }

    QString newCommand = findAlias(command, arguments);
    if (!newCommand.isEmpty()) {
      // Alias should be handled in findAlias
      parseInput(newCommand);
      
    } else if (command.startsWith(_commandProcessor->getCommandSymbol())) {
      // Parse as a Plugin's Command
      command = command.remove(0, 1); // remove the #
    
      qDebug() << "* Parsing command :" << command
	       << arguments << ".";
      parseCommand(command, arguments);
      
    } else {
      // It must be something that should be sent to the socket
      QVariant* qv = new QVariant(current + "\n");
      QStringList sl("SocketWriteData");
      postSession(qv, sl);

    }
  }
}


bool CommandTask::parseCommand(QString command,
			       const QString &arguments) {
  if (command.isEmpty()) command = "help";
    
  // Identify Command
  QMapIterator<QString, QString> i(_commandProcessor->getCommandMapping());
  while (i.hasNext()) {
    if (i.next().key().startsWith(command)) {
      // Command was identified
      command = i.key();
      
      if (!i.value().isNull()) {
	// External commands

	// Relay command to corresponding plugin
	QVariant* qv = new QVariant(arguments);
	QStringList sl;
	sl << i.value();
	postSession(qv, sl);

      } else {
	// Internal Commands
	if (command == "emulate") {
	  // TODO: Add tag parsing
	  findAction(arguments, QStringList("XMLNone"));

	} else if (command == "print") {
	  displayData(arguments);

	} else if (command == "delim") {
	  /*
	  if (arguments.size() == 1 && arguments.at(0) != ' ') {
	    _delim = arguments.at(0);
	    displayData("#delimeter is now " + arguments + "\n");
	  } else
	    displayData("#not allowed\n");
	  */

	} else if (command == "qui") {
	  displayData("#you have to write '#quit' - no less, to quit!\n");
	  
	} else if (command == "quit") {
	  _commandProcessor->emitQuit();

	} else if (command == "version") {
	  QString output = QString("mClient %1, \251 2009 by Jahara\n"
				   "%2")
	    .arg( // %1
#ifdef SVN_REVISION
		 "SVN Revision " + QString::number(SVN_REVISION)
#else
#ifdef MCLIENT_VERSION
		 MCLIENT_VERSION
#else
		 "unknown version"
#endif
#endif
		 )
	    .arg( // %2

#if __STDC__
		 "Compiled " __TIME__ " " __DATE__ "\n"
#else
		 ""
#endif
		 );
	  displayData(output);
	  
	} else if (command == "help") {
	  QString output = "#commands available:\n";
	  QMapIterator<QString, QString>
	    i(_commandProcessor->getCommandMapping());
	  while (i.hasNext())
	    output += QString("#%1\n").arg(i.next().key());
	  displayData(output);

	} else if (command == "beep") {
	  QApplication::beep();

	} else if (command == "alias") {
	  handleAliasCommand(arguments);

	} else if (command == "action") {
	  handleActionCommand(arguments);

	} else if (command == "split") {
	  parseInput(arguments, _commandProcessor->getDelimSymbol());

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
    for (int i = 0; i < repeat; i++) {
      parseInput(arguments);
    }
    return true;
  }

  // Unknown command!
  displayData(QString("#unknown command \"" + command + "\"\n"));
  return false;
}


bool CommandTask::handleAliasCommand(const QString &arguments) {
  AliasManager *aliases = _commandProcessor->getPluginSession()->getAlias();

  if (arguments.isEmpty()) {
    // Display all aliases
    
    QString output;
    if (aliases->count() == 0)
      output = "#no aliases are defined.\n";

    else {
      output = QString("#the following alias%1 defined:\n").arg(aliases->count()==1?" is":"es are");

      // Attach the aliases
      QHashIterator<QString, Alias*> i(aliases->getHash());
      while (i.hasNext()) {
	i.next();
	output += QString("#alias %1=%2\n").arg(i.key(),
						i.value()->command);
      }
    }
    // Display the string
    displayData(output);
    
    return true;
  }
  
  QRegExp aliasRx("^([^\\s=]+)(=?)(.*)");
  if (!aliasRx.exactMatch(arguments)) {
    // Incorrect command syntax
    qDebug() << "* Unknown syntax in Alias' command regular expression";
    return false;
  }
  // Parse the command
  QStringList cmd = aliasRx.capturedTexts();

  qDebug() << cmd;
  QString name(cmd.at(1));
  QString symbol(cmd.at(2));
  QString command(cmd.at(3));

  if (symbol.isEmpty()) {
    
    Alias *alias = aliases->match(name);
    if (alias) {
      // Display alias
      displayData(QString("#alias %1=%2\n").arg(alias->name,
						alias->command));

    } else {
      // Error, no alias exists
      displayData("#unknown alias, cannot show: \""+name+"\"\n");
      return false;
    }

  }
  else if (command.isEmpty()) {

    Alias *alias = aliases->match(name);
    if (alias) {
      // Delete alias
      aliases->remove(name);
      displayData("#deleting alias: "+cmd.at(0)+"\n");

    } else {
      // Error, no alias exists
      displayData("#unknown alias, cannot delete: \""+name+"\"\n");
      return false;

    }
  } else if (symbol == "=") {
    // Create alias
    QString group = QString(); // Null is the default group
    aliases->add(name, command, group);
    displayData("#new alias in group '"
		+(group.isEmpty()?"*":group)+"': "+name+"="+command+"\n");

  }

  return true;
}


QString CommandTask::findAlias(const QString &name,
			       const QString &arguments) {
  qDebug() << "* Alias received an event: " << name << arguments << ".";

  AliasManager *aliases = _commandProcessor->getPluginSession()->getAlias();
  Alias *alias = aliases->match(name);
  
  if (alias) {
    // The current command is an alias
    qDebug() << "found alias" << alias->name << alias->command;
    
    // Create the new command
    QString newCommand = alias->command;
    QStringList tokens = arguments.split(QRegExp("\\s+"),
					 QString::SkipEmptyParts);
    for (int i = 0; i < tokens.size()+1; ++i) {
      QRegExp rx(QString("^(\\$%1)|([^\\\\])(\\$%2)").arg(i).arg(i));
      switch (i) {
      case 0:
	newCommand.replace(rx, "\\2"+arguments);
	break;
      default:
	newCommand.replace(rx, "\\2"+tokens.at(i-1));
	break;
      };
    }
    qDebug() << "* alias command is:" << newCommand;

    return newCommand;
  }

  return 0;
}


bool CommandTask::handleActionCommand(const QString &arguments) {
  ActionManager *actions = _commandProcessor->getPluginSession()->getAction();

  if (arguments.isEmpty()) {
    // Display all Actions

    // Iterate through all the priorities
    QStringList output;
    QMap<int, QMultiHash<QString, Action*>* > hash = actions->getHash();
    QMap<int, QMultiHash<QString, Action*>* >::const_iterator i;
    for (i = hash.constBegin(); i != hash.constEnd(); ++i) {
      qDebug() << "* Looking through actions of priority" << i.key()
	       << "hash has a size of" << i.value()->size();
      
      QHash<QString, Action*>::const_iterator j;
      for (j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
	QString out = QString("#action >%1@%2=%3 {%4} {%5}\n")
	  .arg(j.value()->label,
	       j.value()->pattern.pattern(),
	       j.value()->command,
	       j.value()->group,
	       QString("%1").arg(j.value()->priority));
	
	output << out;
      }
    }
    qDebug() << output;

    // Attach initial text:
    if (output.size() == 0)
      output.prepend("#no actions are defined.\n");
    else
      output.prepend(QString("#the following action%1 defined:\n").arg(output.size()==1?" is":"s are"));
    
    // Display the string
    displayData(output.join(""));
    
    return true;
  }
  
  //                  add/del  toggle      label        pattern
  QRegExp actionRx("^([<|>]?)([+|-]?)([^\\s<>+-]+)\\s+\\{(.+)\\}=(.+)");

  if (!actionRx.exactMatch(arguments)) {
    // Incorrect command syntax
    qDebug() << "* Unknown syntax in Action's command regular expression"
	     << actionRx.errorString();
    return false;
  }
  // Parse the command
  QStringList cmd = actionRx.capturedTexts();
  qDebug() << cmd;

  bool active = cmd.at(2)=="+"?true:false;
  QString label(cmd.at(3));
  QRegExp pattern(cmd.at(4));
  QString command(cmd.at(5));
  QStringList tags("XMLNone");
  
  if (!label.isEmpty()) {
    qDebug() << "trying to make a new action";
    actions->add(label, pattern, command, tags, 0, active);
    qDebug() << "created action";

    displayData("#new action \""+label+"\": "+label+"="+command+"\n");

  }
  
  return true;
}


bool CommandTask::findAction(const QString &pattern, QStringList tags) {
  qDebug() << "* Action received event: " << pattern << tags << ".";

  if (tags.size() > 1) tags.removeAll("XMLAll");

  ActionManager *actions = _commandProcessor->getPluginSession()->getAction();
  Action *action = actions->match(pattern, tags);

  if (action) {
    // Create the new command
    QString newCommand = action->command;
    QStringList tokens = action->pattern.capturedTexts();
    for (int i = 0; i < tokens.size()+1; ++i) {
      QRegExp rx(QString("^(\\$%1)|([^\\\\])(\\$%2)").arg(i).arg(i));
      switch (i) {
      case 0:
	newCommand.replace(rx, "\\2"+pattern);
	break;
      default:
	newCommand.replace(rx, "\\2"+tokens.at(i-1));
	break;
      };
    }
    qDebug() << "* action command is" << newCommand;
    parseInput(newCommand);
    return true;
  }
  
  // No action found, display the text
  displayData(pattern);
  return false;
}


void CommandTask::displayData(const QString &output) {
  QVariant* qv = new QVariant(output);
  QStringList sl("DisplayData");
  postSession(qv, sl);
}

void CommandTask::postSession(QVariant *payload,
				   const QStringList& tags) {
  MClientEventData *med =
    new MClientEventData(payload, tags,
			 _commandProcessor->getPluginSession()->session());
  MClientEvent *me = new MClientEvent(med);

  QApplication::postEvent(_commandProcessor->getPluginSession(), me);
}
