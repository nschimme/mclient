#include <QDebug>
#include <QVariant>

#include "ActionPlugin.h"

#include "MClientEvent.h"
#include "CommandManager.h"

Q_EXPORT_PLUGIN2(actionplugin, ActionPlugin)

class Action {
 public:
  Action(const QString &label, const QRegExp &pattern, const QString &command,
	 const QStringList &tags, const QString &group=0, uint priority=0) {
    _label = label;
    _pattern = pattern;
    _command = command;
    _group = group;
    _priority = priority;
    _tags = tags;
  }
  
  QString& label(const QString &label=0) {
    if (label != 0) _label = label;
    return _label;
  }

  QRegExp& pattern() {
    return _pattern;
  }

  QString& command(const QString &command=0) {
    if (command != 0) _command = command;
    return _command;
  }

  QString& group(const QString &group=0) {
    if (group != 0) _group = group;
    return _group;
  }

  uint& priority(const int priority=-1) {
    if (priority >= 0) _priority = (uint)priority;
    return _priority;
  }
  
 private:
  QString _label;
  QRegExp _pattern;
  QString _command;
  QString _group;
  QStringList _tags;
  uint _priority;
};

struct ActionHolder {
  QMultiMap<int, QMultiHash<QString, Action*> > actions;
  QMultiHash<QString, Action*> groups;
  QMultiHash<QString, Action*> tags;
};

ActionPlugin::ActionPlugin(QObject *parent) 
        : MClientPlugin(parent) {
    _shortName = "actionplugin";
    _longName = "Action Plugin";
    _description = "Handles Actions";
    //_dependencies.insert("terrible_test_api", 1);
//    _implemented.insert("some_other_api",1);
    _dataTypes << "ActionCommand" << "XMLAll";
    _configurable = false;
    _configVersion = "2.0";

    // register commands
    QStringList commands;
    commands << _shortName
	     << "action" << "ActionCommand";
    CommandManager::instance()->registerCommand(commands);
}


ActionPlugin::~ActionPlugin() {
    stopAllSessions();
    saveSettings();
}

void ActionPlugin::customEvent(QEvent* e) {
    if(!e->type() == 10001) return;
    
    MClientEvent* me;
    me = static_cast<MClientEvent*>(e);

    if(me->dataTypes().contains("XMLAll")) {
      QString action = me->payload()->toString();
      parseAction(action, me->dataTypes(), me->session());

    } else if (me->dataTypes().contains("ActionCommand")) {
      QString arguments = me->payload()->toString();
      handleCommand(arguments, me->session());

    }
}


bool ActionPlugin::handleCommand(const QString &arguments,
				const Session &session) {
  // Grab the session's Actions
  ActionHolder *h = _sessions[session];

  if (arguments.isEmpty()) {
    // Display all Actions

    // Iterate through all the priorities
    QStringList output;
    QMapIterator<int, QMultiHash<QString, Action*> > i(h->actions);
    while (i.hasNext()) {
      i.next();
      qDebug() << "* Looking through actions of priority" << i.key();
      
      QHashIterator<QString, Action*> j(i.value());
      while (j.hasNext()) {
	j.next();
	output << QString("#action >%1%2@%3=%4\n").arg(j.value()->label(),
						       QString(j.value()->priority()),
						       j.value()->group(),
						       j.value()->command());
	
      }
    }

    // Attach initial text:
    if (output.size() == 0)
      output.prepend("#no actions are defined.\n");
    else
      output.prepend(QString("#the following action%1 defined\n").arg(h->actions.size()==1?" is":"es are"));
    
    // Display the string
    displayData(output.join(""), session);
    
    return true;
  }
  
  //                  add/del  toggle      label         pattern
  QRegExp actionRx("^([<|>]?)([\\+|-]?)([^\\s<>\\+-]+) (\\{.*\\})"
		   "[ \\{(\\d+)\\}]*[ \\{([^}]+)\\}]*[ \\{([^}]+)\\}]*");
  //                 priority      group       tag(s)

  if (!actionRx.exactMatch(arguments)) {
    // Incorrect command syntax
    qDebug() << "* Unknown syntax in Action's command regular expression"
	     << actionRx.errorString();
    return false;
  }
  // Parse the command
  QStringList cmd = actionRx.capturedTexts();

  qDebug() << cmd;

  return true;
}


bool ActionPlugin::parseAction(const QString &text, QStringList tags,
			       const Session &session) {
  ActionHolder *h = _sessions[session];

  qDebug() << "* ActionPlugin got an event: " << text << tags << ".";

  if (tags.size() > 1) tags.removeAll("XMLAll");

  // Iterate through all the priorities
  QMultiMap<int,
    QMultiHash<QString, Action*> >::const_iterator i = h->actions.constBegin();
  while (i != h->actions.constEnd()) {
    qDebug() << "* Looking through actions of priority" << i.key();
    
    // Iterate through the tag in question
    QString tag = tags.at(0);
    QMultiHash<QString, Action*>::const_iterator j = i.value().find(tag);
    while (j != i.value().end() && j.key() == tag) {
      
      if (j.value()->pattern().indexIn(text)) {
	// Pattern matched
	qDebug() << "found action" << j.key()
		 << j.value()->command()
		 << "for tag" << tag;
	qDebug() << j.value()->pattern().capturedTexts();
	
	return true;
      }
      ++j;
    }
    ++i;
  }
  // No action found, display the text
  displayData(text, session);
  return false;
}


void ActionPlugin::configure() {
}


const bool ActionPlugin::loadSettings() {
  return true;
}

        
const bool ActionPlugin::saveSettings() const {
  return true;
}


const bool ActionPlugin::startSession(QString s) {
  ActionHolder *actions = new ActionHolder;
  _sessions.insert(s, actions);
  _runningSessions << s;
  return true;
}

const bool ActionPlugin::stopSession(QString s) {
  _sessions.remove(s); // TODO: improve, delete individual actions
  int removed = _runningSessions.removeAll(s);
  return removed!=0?true:false;
}

void ActionPlugin::displayData(const QString &output,
			      const QString &session) {
  QVariant* qv = new QVariant(output);
  QStringList sl;
  sl << "XMLDisplayData";
  postEvent(qv, sl, session);  
}
