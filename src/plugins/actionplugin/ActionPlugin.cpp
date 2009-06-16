#include <QDebug>
#include <QVariant>

#include "ActionPlugin.h"

#include "MClientEvent.h"
#include "MClientEngineEvent.h"

#include "CommandManager.h"
#include "PluginManager.h"
#include "PluginSession.h"

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
  QMultiMap<int, QMultiHash<QString, Action*>* > actions;
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
    _receivesDataTypes << "ActionCommand" << "XMLAll";
    _deliversDataTypes << "DisplayData";
    _configurable = false;
    _configVersion = "2.0";
}


ActionPlugin::~ActionPlugin() {
    saveSettings();
}

void ActionPlugin::customEvent(QEvent* e) {
  if (e->type() == 10000)
    engineEvent(e);
  else if (e->type() == 10001) {
    
    MClientEvent* me = static_cast<MClientEvent*>(e);

    if(me->dataTypes().contains("XMLAll")) {
      QString action = me->payload()->toString();
      parseAction(action, me->dataTypes());

    } else if (me->dataTypes().contains("ActionCommand")) {
      QString arguments = me->payload()->toString();
      handleCommand(arguments);

    }
  }
  else 
    qDebug() << "* ActionPlugin got a customEvent of type" << e->type();
}


bool ActionPlugin::handleCommand(const QString &arguments) {
  if (arguments.isEmpty()) {
    // Display all Actions

    // Iterate through all the priorities
    QStringList output;
    QMap<int, QMultiHash<QString, Action*>* >::const_iterator i;
    for (i = h->actions.constBegin(); i != h->actions.constEnd(); ++i) {
      qDebug() << "* Looking through actions of priority" << i.key()
	       << "hash has a size of" << i.value()->size();
      
      QHash<QString, Action*>::const_iterator j;
      for (j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
	QString out = QString("#action >%1@%2=%3 {%4} {%5}\n").arg(j.value()->label(),
								   j.value()->group(),
								   j.value()->command(),
								   j.key(),
								   QString("%1").arg(j.value()->priority()));
	
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

  QString label(cmd.at(3));
  QRegExp pattern(cmd.at(4));
  QString command(cmd.at(5));
  QStringList tags("XMLNone");
  
  if (!label.isEmpty()) {
    qDebug() << "trying to make a new action";
    Action *action = new Action(label, pattern, command, tags);
    qDebug() << "created action";

    bool newHash = false;
    QMultiHash<QString, Action*> *actions;
    if (h->actions.contains(0)) {
      actions = h->actions.value(0);
      qDebug() << "received hash of size" << actions->size();
      
    } else {
      actions = new QMultiHash<QString, Action*>;
      qDebug() << "created NEW hash";
      newHash = true;
    }

    foreach(QString tag, tags) {
      qDebug() << "adding for tag" << tag;
      actions->insert(tag, action);
      qDebug() << "inserted action! hash is now size of" << actions->size();
      displayData("#new action \""+label+"\": "+label+"="+command+"\n");
    }

    if (newHash) h->actions.insert(0, actions);

  }
  
  return true;
}


bool ActionPlugin::parseAction(const QString &text, QStringList tags) {
  qDebug() << "* ActionPlugin got an event: " << text << tags << ".";

  if (tags.size() > 1) tags.removeAll("XMLAll");

  // Iterate through all the priorities
  QMultiMap<int,
    QMultiHash<QString, Action*>* >::const_iterator i = h->actions.constBegin();
  while (i != h->actions.constEnd()) {
    qDebug() << "* Looking through actions of priority" << i.key();
    
    // Iterate through the tag in question
    QString tag = tags.at(0);
    qDebug() << "looking at tag" << tag;
    QMultiHash<QString, Action*>::iterator j = i.value()->find(tag);
    while (j != i.value()->end() && j.key() == tag) {

      qDebug() << "matching against action" << j.value()->label();
      
      if (j.value()->pattern().indexIn(text) >= 0) {
	// Pattern matched
	qDebug() << "found action" << j.value()->label()
		 << j.value()->command()
		 << "for tag" << tag;
	qDebug() << j.value()->pattern().capturedTexts();
	
	// TODO: replace command stuff with capturedTexts().
	QString command = j.value()->command();
	_pluginSession->getManager()->getCommand()->parseInput(command,
							       _session);

	return true;

      } else {
	qDebug() << j.value()->pattern().errorString();

      }
      ++j;
    }
    ++i;
  }
  // No action found, display the text
  displayData(text);
  return false;
}


void ActionPlugin::configure() {
}


bool ActionPlugin::loadSettings() {
    // register commands
    QStringList commands;
    commands << _shortName
	     << "action" << "ActionCommand";
    _pluginSession->getManager()->getCommand()->registerCommand(commands);

  return true;
}

        
bool ActionPlugin::saveSettings() const {
  return true;
}


bool ActionPlugin::startSession(QString) {
  h = new ActionHolder;
  return true;
}

bool ActionPlugin::stopSession(QString) {
  delete h;
  return true;
}

void ActionPlugin::displayData(const QString &output) {
  QVariant* qv = new QVariant(output);
  QStringList sl("DisplayData");
  postSession(qv, sl);  
}
