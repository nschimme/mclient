#include <QDebug>
#include <QVariant>

#include "AliasPlugin.h"

#include "MClientEvent.h"
#include "CommandManager.h"

Q_EXPORT_PLUGIN2(aliasplugin, AliasPlugin)

class Alias {
 public:
  Alias(const QString& name, const QString &command, const QString &group=0) {
    _name = name;
    _command = command;
    _group = group;
  }
  
  QString& name(const QString &name=0) {
    if (name != 0) _name = name;
    return _name;
  }

  QString& command(const QString &command=0) {
    if (command != 0) _command = command;
    return _command;
  }

  QString& group(const QString &group=0) {
    if (group != 0) _group = group;
    return _group;
  }
  
 private:
  QString _name;
  QString _command;
  QString _group;
};

struct AliasHolder {
  QHash<QString, Alias*> aliases;
  QMultiHash<QString, Alias*> groups;
};

AliasPlugin::AliasPlugin(QObject *parent) 
        : MClientPlugin(parent) {
    _shortName = "aliasplugin";
    _longName = "Alias Plugin";
    _description = "Handles aliases";
    //_dependencies.insert("terrible_test_api", 1);
//    _implemented.insert("some_other_api",1);
    _dataTypes << "AliasInput" << "AliasCommand";
    _configurable = false;
    _configVersion = "2.0";

    // register commands
    QStringList commands;
    commands << _shortName
	     << "alias" << "AliasCommand";
    CommandManager::instance()->registerCommand(commands);
}


AliasPlugin::~AliasPlugin() {
    stopAllSessions();
    saveSettings();
}

void AliasPlugin::customEvent(QEvent* e) {
    if(!e->type() == 10001) return;
    
    MClientEvent* me;
    me = static_cast<MClientEvent*>(e);

    if(me->dataTypes().contains("AliasInput")) {
      QString alias = me->payload()->toString();
      parseInput(alias, me->session());

    } else if (me->dataTypes().contains("AliasCommand")) {
      QString arguments = me->payload()->toString();
      handleCommand(arguments, me->session());

    }
}


bool AliasPlugin::handleCommand(const QString &arguments,
				const Session &session) {
  // Grab the session's aliases
  AliasHolder *h = _sessions[session];

  if (arguments.isEmpty()) {
    // Display all aliases
    
    QString output;
    if (h->aliases.size() == 0)
      output = "#no aliases are defined.\n";

    else {
      output = QString("#the following alias%1 defined:\n").arg(h->aliases.size()==1?" is":"es are");

      // Attach the aliases
      QHashIterator<QString, Alias*> i(h->aliases);
      while (i.hasNext()) {
	i.next();
	output += QString("#alias %1=%2\n").arg(i.key(),
						i.value()->command());
      }
    }
    // Display the string
    displayData(output, session);
    
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

    if (h->aliases.contains(name)) {
      // Display alias
      displayData(QString("#alias %1=%2\n").arg(name,
						h->aliases[name]->command()),
		  session);

    } else {
      // Error, no alias exists
      displayData("#unknown alias, cannot show: \""+name+"\"\n",
		  session);
      return false;
    }

  }
  else if (command.isEmpty()) {
    if (h->aliases.contains(name)) {
      // Delete alias
      h->aliases.remove(name);
      displayData("#deleting alias: "+cmd.at(0)+"\n",
		  session);

    } else {
      // Error, no alias exists
      displayData("#unknown alias, cannot delete: \""+name+"\"\n",
		  session);
      return false;

    }
  } else if (symbol == "=") {
    // Create alias
    QString group = QString(); // Null is the default group
    Alias *alias = new Alias(name, command);
    h->aliases.insert(name, alias);
    if (group.isEmpty())
      group = "*";
    displayData("#new alias in group '"+group+"': "+name+"="+command+"\n",
		session);

  }

  return true;
}


void AliasPlugin::parseInput(const QString &input,
			     const Session &session) {
  AliasHolder *h = _sessions[session];

  Command command(input);
  QStringList arguments;
  QRegExp whitespace("\\s+");
  int whitespaceIndex = input.indexOf(whitespace);
  if (whitespaceIndex >= 0) {
    command = input.mid(whitespaceIndex);
    arguments = input.mid(whitespaceIndex+1).split(whitespace,
						   QString::SkipEmptyParts);
  }

  qDebug() << "* AliasPlugin got an event: " << command << arguments << ".";

  if (h->aliases.contains(command)) {
    // The current command is an alias
    qDebug() << "found alias" << command << h->aliases[command]->command();
    CommandManager::instance()->parseInput(h->aliases[command]->command(),
					   session);

  } else {
    // Not an alias, send it to the socket
    QVariant* qv = new QVariant(input + "\n");
    QStringList sl("SendToSocketData");
    postEvent(qv, sl, session);
  }

}


void AliasPlugin::configure() {
}


const bool AliasPlugin::loadSettings() {
  return true;
}

        
const bool AliasPlugin::saveSettings() const {
  return true;
}


const bool AliasPlugin::startSession(QString s) {
  AliasHolder *aliases = new AliasHolder;
  _sessions.insert(s, aliases);
  _runningSessions << s;
  return true;
}

const bool AliasPlugin::stopSession(QString s) {
  _sessions.remove(s); // TODO: improve, delete individual aliases
  int removed = _runningSessions.removeAll(s);
  return removed!=0?true:false;
}

void AliasPlugin::displayData(const QString &output,
			      const QString &session) {
  QVariant* qv = new QVariant(output);
  QStringList sl("DisplayData");
  postEvent(qv, sl, session);  
}
