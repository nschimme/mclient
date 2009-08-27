#include "AbstractCommandTask.h"

#include "PluginSession.h"
#include "CommandProcessor.h"
#include "AliasManager.h"
#include "ActionManager.h"
#include "CommandEntry.h"

#include "MClientEvent.h"

#include <QApplication>
#include <QDebug>
#include <QVariant>

AbstractCommandTask::AbstractCommandTask(CommandProcessor *ps, QObject *parent)
  : QThread(parent), _commandProcessor(ps) {

  _queue = QStringList();
  _stack = 0;

  _aliasManager = _commandProcessor->getPluginSession()->getAlias();
  _actionManager = _commandProcessor->getPluginSession()->getAction();
}


AbstractCommandTask::~AbstractCommandTask() {
}


void AbstractCommandTask::customEvent(QEvent *e) {
  if (e->type() == 10001) {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    if (me->dataTypes().contains("SocketConnected")) {
      _socketOpen = true;

    }
    else if (me->dataTypes().contains("SocketDisconnected")) {
      _socketOpen = false;
      
    }
  }
}


void AbstractCommandTask::run() {
  qDebug() << "AbstractCommandTask run" << QThread::currentThread();
  exec();
  qDebug() << "AbstractCommandTask" << QThread::currentThread() << "done";
}


void AbstractCommandTask::parseUserInput(const QString &input) {
  // Add current input to the bottom of the queue
  _queue.prepend(input);
  //qDebug() << "* adding" << _queue.first() << "to queue";
  processStack();

}

bool AbstractCommandTask::processStack() {
  // Reinitialize the stack
  _stack = 0;

  while (!_queue.isEmpty()) {
    qDebug() << "* Command queue:" << _stack << _queue;

    if (_stack++ > 100) {
      _queue.clear();
      qWarning() << "! Stack overflow!";
      displayData("#error: stack overflow.\n");
      return false;
    }

    // Pop stack
    QString current(_queue.takeLast());
    //qDebug() << "* popping" << current << "from queue";

    QString command, arguments;    
    QRegExp rx("^\\s*(\\S+)(?:\\s+(.+))?");
    int rxIndex = rx.indexIn(current);
    if (rxIndex >= 0) {
      command = rx.cap(1);
      arguments = rx.cap(2);
    }

    // First, find if there is a matching alias
    if (findAlias(command, arguments)) continue;

    // Otherwise, see if it is a command
    else if (findCommand(command, arguments)) continue;

    // Lastly, it must be something that needs to be sent to the socket
    else if (_socketOpen) {
      parseArguments(current);
      qDebug() << "* writing" << current << "to socket";

      QVariant* qv = new QVariant(current + "\n");
      QStringList sl("SocketWriteData");
      postSession(qv, sl);
      
    }
    else {
      displayData("#no open connections. "
		  "Use '\033[1m#connect\033[0m' to open a connection.\n");

    }
  }
  return true;
}


const QString& AbstractCommandTask::parseArguments(QString &arguments,
						   CommandEntryType type) {
  if (type == CMD_ONE_LINE) {
    int lineEnd;
    if ((lineEnd = arguments.indexOf("\n")) != -1) {
      _queue.append(arguments.mid(lineEnd + 1));
      arguments.truncate(lineEnd);
      qDebug() << "arguments truncated to" << arguments << _queue;
    }

  } else {
    unsigned int i, leftCount = 0, rightCount = 0, escapedChar = 0;
    for (i = 0; i < (unsigned int) arguments.length(); i++) {
      switch (arguments.at(i).toAscii()) {
      case '\\':
	escapedChar = i;
	break;

      case '{':
	if (escapedChar != i-1 && leftCount >= rightCount) {
	  leftCount++;
	}
	break;

      case '}':
	if (escapedChar != i-1) {
	  rightCount++;
	  if (rightCount > leftCount) {
	    qDebug() << "! WTF?";

	  } else if (leftCount == rightCount) {
	    _queue.append(arguments.mid(i));
	    arguments.truncate(i);
	    
	  }
	}
	break;

      case '\n':
	if (leftCount == 0) {
	  _queue.append(arguments.mid(i));
	  arguments.truncate(i);

	}
	break;

      };
    }
  }

  return arguments;
}


bool AbstractCommandTask::findCommand(const QString &rawCommand,
				      QString &arguments) {
  if (!rawCommand.startsWith(_commandProcessor->getCommandSymbol()))
    return false;
  
  // Remove the command symbol
  QString command(rawCommand);
  command.remove(0, 1);

  // Identify Command
  CommandMapping map = _commandProcessor->getCommandMapping();
  CommandMapping::const_iterator i;
  for (i = map.constBegin(); i != map.constEnd(); ++i) {
    if (i.key().startsWith(command)) {
      // Command was identified

      // Parse the arguments depending on if the command is
      // one- or two-lined
      parseArguments(arguments, i.value()->commandType());

      if (!i.value()->pluginName().isEmpty()) {
	// External commands, relay command to corresponding plugin
	QVariant* qv = new QVariant(arguments);
	QStringList sl;
	sl << i.value()->dataType();
	postSession(qv, sl);
    	return true;

      } else {
	// Internal Commands
	command = i.key();

	if (command == "emulate") {
	  // TODO: Add tag parsing
	  findAction(arguments, QStringList("XMLNone"));
    
	} else if (command == "print") {
	  // Unescape arguments and display
	  QString temp(arguments);
	  QRegExp rx("\\\\(.)");
	  int pos = 0;
	  while ((pos = rx.indexIn(temp, pos)) != -1) {
	    switch (rx.cap(1).at(0).toAscii()) {
	    case 'r':
	      temp.replace(pos, 2, "\r");
	      break;
	    case 'n':
	      temp.replace(pos, 2, "\n");
	      break;
	    case 't':
	      temp.replace(pos, 2, "\t");
	      break;
	    default:
	      temp.remove(pos--, 1);
	    };
	    pos += rx.matchedLength();
	  }
	  displayData(temp);

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
	  QString output = "\033[1;4m#commands available:\033[0m\n";
	  for (i = map.constBegin(); i != map.constEnd(); ++i) {
	    output += QString("\033[1m#%1\033[0m%2\n")
	      .arg(i.key(), -15, ' ') // pad 15 characters
	      .arg(i.value()->help());
	  }
	  displayData(output);

	} else if (command == "beep") {
	  QApplication::beep();

	} else if (command == "alias") {
	  handleAliasCommand(arguments);

	} else if (command == "action") {
	  handleActionCommand(arguments);

	} else if (command == "split") {
	  QChar splitChar = _commandProcessor->getDelimSymbol();
	  // QString::SkipEmptyParts
	  QListIterator<QString> i(arguments.split(splitChar));
	  for (i.toBack(); i.hasPrevious();) {
	    _queue.append(i.previous());
	  }

	}
      }
      qDebug() << "* Command " << command << "was run";
      return true;

    }
  }

  // Check if it is a repeat
  bool ok;
  int repeat = command.toInt(&ok);
  if (ok) {
    for (int i = 0; i < repeat; i++)
      parseUserInput(arguments);
    return true;

  }

  // Unknown command!
  parseArguments(arguments);
  displayData(QString("#unknown command \"" + command + "\"\n"));
  return false;

}
 

bool AbstractCommandTask::handleAliasCommand(const QString &arguments) {
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
    if (_verbose)
      displayData("#new alias in group '"
		  +(group.isEmpty()?"*":group)+"': "+name+"="+command+"\n");

  }

  return true;
}


bool AbstractCommandTask::findAlias(const QString &name,
				    QString &arguments) {
  // Try to find the alias
  Alias *alias = _aliasManager->match(name);
  if (alias) {
    // The current command is an alias
    qDebug() << "* found alias" << alias->name << alias->command;

    // Split the arguments to only one line, add the rest to the queue
    parseArguments(arguments);

    // Create the new command
    QString newCommand = alias->command;
    QStringList tokens = arguments.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    for (int i = 0; i <= tokens.size(); ++i) {
      QRegExp rx(QString("^\\$%1|([^\\\\])\\$%1").arg(i));
      switch (i) {
      case 0:
	newCommand.replace(rx, "\\1"+arguments);
	break;
      default:
	newCommand.replace(rx, "\\1"+tokens.at(i-1));
	break;
      };
    }
    qDebug() << "* alias command is:" << newCommand;
    
    displayData("[" + newCommand + "]\n");
    parseUserInput(newCommand);
    return true;

  }

  return false;
}
  
bool AbstractCommandTask::handleActionCommand(const QString &arguments) {
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
	       j.value()->tags.join(","),
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


bool AbstractCommandTask::findAction(const QString &pattern, QStringList tags) {
  if (tags.size() > 1) tags.removeAll("XMLAll");

  ActionManager *actions = _commandProcessor->getPluginSession()->getAction();
  Action *action = actions->match(pattern, tags);

  if (action) {
    // Create the new command
    QString newCommand = action->command;
    QStringList captures = action->pattern.capturedTexts();
    for (int i = 0; i < captures.size(); ++i) {
      // Match against non-escaped $ variables
      QRegExp rx(QString("^\\$%1|([^\\\\])\\$%1").arg(i));
      switch (i) {
      case 0:
	/*
	if (pattern.contains("\n")) {
	  // Remove newlines from $0
	  newCommand.replace(rx, captures[i].replace(QString("\n"),
						     QString(" ")));
	  continue;
	}
	*/
      default:
	newCommand.replace(rx, "\\1"+captures.at(i));
	break;
      };
    }

    // Unescape $s
    newCommand.replace(QRegExp("\\\\\\$(\\d+)"), "$\\1");

    qDebug() << "* action command is" << newCommand;

    // Display text
    displayData(pattern);

    parseUserInput(newCommand);
    return true;
  }
  
  // No action found, display the text
  displayData(pattern);
  return false;
}


void AbstractCommandTask::parseMudOutput(const QString &output,
				 const QStringList &tags) {
  // match tag blocks
  findAction(output, tags);
  processStack();

}


void AbstractCommandTask::displayData(const QString &output) {
  QVariant* qv = new QVariant(output);
  QStringList sl("DisplayData");
  postSession(qv, sl);
}

void AbstractCommandTask::postSession(QVariant *payload,
			      const QStringList& tags) {
  MClientEventData *med =
    new MClientEventData(payload, tags,
			 _commandProcessor->getPluginSession()->session());
  MClientEvent *me = new MClientEvent(med);

  QApplication::postEvent(_commandProcessor->getPluginSession(), me);
}
