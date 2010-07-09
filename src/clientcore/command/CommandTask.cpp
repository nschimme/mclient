#include "CommandTask.h"

#include "PluginSession.h"
#include "CommandProcessor.h"
#include "AliasManager.h"
#include "ActionManager.h"
#include "CommandEntry.h"

#include "MClientEvent.h"

#include <QObject>
#include <QApplication>
#include <QDebug>
#include <QVariant>

CommandTask::CommandTask(CommandProcessor *ps)
  : QObject(0), _commandProcessor(ps) {

  _queue = QStringList();
  _stack = 0;
  _verbose = false;
  _socketOpen = false;

  _aliasManager = _commandProcessor->getPluginSession()->getAlias();
  _actionManager = _commandProcessor->getPluginSession()->getAction();

  // Signal/Slots
  connect(ps, SIGNAL(socketOpen(bool)),
	  SLOT(socketOpen(bool)));
  connect(ps, SIGNAL(parseUserInput(const QString&)),
	  SLOT(parseUserInput(const QString&)));
  connect(ps, SIGNAL(parseMudOutput(const QString &, const QStringList &)),
	  SLOT(parseMudOutput(const QString &, const QStringList &)));

  qDebug() << "* CommandTask created with thread" << QThread::currentThread();
}


CommandTask::~CommandTask() {
}


void CommandTask::parseUserInput(const QString &input) {
  // Add current input to the bottom of the queue
  _queue.prepend(input);
  //qDebug() << "* adding" << _queue.first() << "to queue";

  processStack();
}

bool CommandTask::processStack() {
  // Reinitialize the stack
  _stack = 0;

  while (!_queue.isEmpty()) {
    qDebug() << "* Command queue:" << _stack << _queue;

    if (_stack++ > 100) {
      _queue.clear();
      qWarning() << "! Stack overflow!";
      displayData("#error: stack overflow.\r\n");
      continue;
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
		  "Type \033[1m#connect\033[0m to open a connection.\r\n");

    }
  }

  return true;
}


const QString& CommandTask::parseArguments(QString &arguments,
						   CommandEntryType type) {
  if (type == CMD_ONE_LINE) {
    QRegExp rx("\r?\n");
    int lineEnd;
    if ((lineEnd = rx.indexIn(arguments)) != -1) {
      _queue.append(arguments.mid(lineEnd + rx.matchedLength()));
      arguments.truncate(lineEnd);
      qDebug() << "arguments truncated to" << arguments << _queue;
    }

  } else {
    int i, leftCount = 0, rightCount = 0, escapedChar = 0;
    for (i = 0; i < arguments.length(); i++) {
      //qDebug() << arguments[i] << leftCount << rightCount << escapedChar;
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
	    // TODO: throw error

	  }
	  else if (leftCount == rightCount) {
	    qDebug() << "* detected finish";
	    QString nextCommand = arguments.mid(i+1);
	    QRegExp rx("\r?\n");
	    int pos;
	    if ((pos = rx.indexIn(nextCommand)) != -1) {
	      qDebug() << "* stripping" << nextCommand.left(pos + rx.matchedLength());
	      // TODO: throw error
	      nextCommand.remove(0, pos + rx.matchedLength());
	    }
	    if (!nextCommand.isEmpty())
	      _queue.append(nextCommand);
	    arguments.truncate(i+1);
	    
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


bool CommandTask::findCommand(const QString &rawCommand,
				      QString &arguments) {
  if (!rawCommand.startsWith(_commandProcessor->getCommandSymbol()))
    return false;
  
  // Remove the command symbol
  QString command(rawCommand);
  command.remove(0, 1);

  if (command.isEmpty()) command = "help";

  // Identify Command
  const CommandMapping &map = _commandProcessor->getCommandMapping();
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
	QStringList sl(i.value()->dataType());
	postSession(qv, sl);

	if (i.value()->locking()) {
	  // External command handler
	  qDebug() << "* LOCKING" << i.value()->pluginName();
	  _semaphore.acquire();
	  qDebug() << "* UNLOCKING" << i.value()->pluginName();

	}
	return true;

      } else if (internalCommand(i.key(), arguments)) {
	qDebug() << "* Internal command " << command << "was run";
	return true;

      }
    }
  }

  // Maybe there are more commands?
  parseArguments(arguments);

  // Unknown command!
  displayData(QString("#unknown command \"" + command + "\"\r\n"));
  return true;
}


bool CommandTask::internalCommand(const QString &command,
				  QString &arguments) {
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
      displayData("#delimeter is now " + arguments + "\r\n");
      } else
      displayData("#not allowed\r\n");
    */

  } else if (command == "qui") {
    displayData("#you have to write '#quit' - no less, to quit!\r\n");
	  
  } else if (command == "quit") {
    _commandProcessor->emitQuit();

  } else if (command == "version") {
    QString output = QString("mClient %1, \251 2009 by Jahara\r\n"
			     "%2"
			     "%3")
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
	   "Compiled " __TIME__ " " __DATE__ "\r\n"
#else
	   ""
#endif
	   )
      .arg( // %3
	   tr("Based on Qt %1 (%2 bit)\r\n")
	   .arg(QLatin1String(QT_VERSION_STR),
		QString::number(QSysInfo::WordSize))
	   );
    displayData(output);
	  
  } else if (command == "help") {
    QString output = "\033[1;4m#commands available:\033[0m\r\n";
    const CommandMapping &map = _commandProcessor->getCommandMapping();
    CommandMapping::const_iterator i;
    for (i = map.constBegin(); i != map.constEnd(); ++i) {
      output += QString("\033[1m#%1\033[0m%2\r\n")
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
    const QChar &splitChar = _commandProcessor->getDelimSymbol();
    // QString::SkipEmptyParts
    QListIterator<QString> i(arguments.split(splitChar));
    for (i.toBack(); i.hasPrevious();) {
      _queue.append(i.previous());
    }

  } else if (command == "session") {
    // TODO: finish this command (start/stop)
    //_commandProcessor->getPluginSession()->stopSession();
    
  } else if (QRegExp("^\\d+$").exactMatch(command)) {
    if (!arguments.isEmpty()) {
      // Check if it is a repeat
      bool ok;
      int repeat = command.toInt(&ok);
      if (ok) {
	QStringList newCommand;
	for (int i = 0; i < repeat; i++) {
	  newCommand << arguments;
	}
	parseUserInput(newCommand.join("\n"));
      }
    } else {
      // TODO: History repeat
      qDebug() << "TODO: History repeat";
    }
  }

  return true;
}


bool CommandTask::handleAliasCommand(const QString &arguments) {
  AliasManager *aliases = _commandProcessor->getPluginSession()->getAlias();

  if (arguments.isEmpty()) {
    // Display all aliases
    
    QString output;
    if (aliases->count() == 0)
      output = "#no aliases are defined.\r\n";

    else {
      output = QString("#the following alias%1 defined:\r\n").arg(aliases->count()==1?" is":"es are");

      // Attach the aliases
      QHashIterator<QString, Alias*> i(aliases->getHash());
      while (i.hasNext()) {
	i.next();
	output += QString("#alias %1=%2\r\n").arg(i.key(),
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
      displayData(QString("#alias %1=%2\r\n").arg(alias->name,
						alias->command));

    } else {
      // Error, no alias exists
      displayData("#unknown alias, cannot show: \""+name+"\"\r\n");
      return false;
    }

  }
  else if (command.isEmpty()) {

    Alias *alias = aliases->match(name);
    if (alias) {
      // Delete alias
      aliases->remove(name);
      displayData("#deleting alias: "+cmd.at(0)+"\r\n");

    } else {
      // Error, no alias exists
      displayData("#unknown alias, cannot delete: \""+name+"\"\r\n");
      return false;

    }
  } else if (symbol == "=") {
    // Create alias
    QString group = QString(); // Null is the default group
    aliases->add(name, command, group);
    if (_verbose)
      displayData("#new alias in group '"
		  +(group.isEmpty()?"*":group)+"': "+name+"="+command+"\r\n");

  }

  return true;
}


bool CommandTask::findAlias(const QString &name,
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
    
    if (_verbose) displayData("[" + newCommand + "]\r\n");
    parseUserInput(newCommand);
    return true;

  }

  return false;
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
	QString out = QString("#action >%1@%2=%3 {%4} {%5}\r\n")
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
      output.prepend("#no actions are defined.\r\n");
    else
      output.prepend(QString("#the following action%1 defined:\r\n").arg(output.size()==1?" is":"s are"));
    
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

    displayData("#new action \""+label+"\": "+label+"="+command+"\r\n");

  }
  
  return true;
}


bool CommandTask::findAction(const QString &pattern, QStringList tags) {
  if (tags.size() > 1) tags.removeAll("XMLAll");

  ActionManager *actions = _commandProcessor->getPluginSession()->getAction();
  Action *action = actions->match(pattern, tags);

  if (action) {
    // Create the new command and substituting text
    QString newCommand = action->command;
    QString newText;
    if (action->substitute) newText = action->text;
    QStringList captures = action->pattern.capturedTexts();
    for (int i = 0; i < captures.size(); ++i) {
      // Match against non-escaped $ variables
      QRegExp rx(QString("^\\$%1|([^\\\\])\\$%1").arg(i));
      newCommand.replace(rx, "\\1"+captures.at(i));
      if (action->substitute)
	newText.replace(rx, "\\1"+captures.at(i));
    }

    // Unescape $s
    newCommand.replace(QRegExp("\\\\\\$(\\d+)"), "$\\1");

    qDebug() << "* action command is" << newCommand;

    // Display text
    if (!action->substitute) newText = pattern;
    if (tags.contains("XMLPrompt"))
      displayPrompt(newText);
    else
      displayData(newText);

    parseUserInput(newCommand);
    return true;
  }
  
  // No action found, display the text
    if (tags.contains("XMLPrompt"))
      displayPrompt(pattern);
    else
      displayData(pattern);
  return false;
}


void CommandTask::parseMudOutput(const QString &output,
				 const QStringList &tags) {
  // match tag blocks
  findAction(output, tags);
  processStack();

}


void CommandTask::displayData(const QString &output) {
  QVariant* qv = new QVariant(output);
  QStringList sl("DisplayData");
  postSession(qv, sl);
}


void CommandTask::displayPrompt(const QString &output) {
  QVariant* qv = new QVariant(output);
  QStringList sl("DisplayPrompt");
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


void CommandTask::socketOpen(bool open) {
  _socketOpen = open;
}
