#include "ActionManager.h"

#include <QApplication>
#include <QDebug>

ActionManager::ActionManager(QObject* parent)
  : QObject(parent) {

  qDebug() << "ActionManager created with thread:" << this->thread();
}

ActionManager::~ActionManager() {
}


Action* ActionManager::match(const QString &pattern,
			     const QStringList &tags) const {
  // Iterate through all the priorities
  QMultiMap<int,
    QMultiHash<QString, Action*>* >::const_iterator i = _actions.constBegin();
  while (i != _actions.constEnd()) {
    qDebug() << "* Looking through actions of priority" << i.key();
    
    QStringList::const_iterator tag = tags.constBegin();
    while (tag != tags.constEnd()) {

      qDebug() << "looking at tag" << *tag;
      QMultiHash<QString, Action*>::const_iterator j
	= i.value()->constFind(*tag);
      while (j != i.value()->constEnd() && j.key() == *tag) {

	// Only match active actions
	if (!j.value()->active) {
	  qDebug() << "action not active" << j.value()->label;
	  ++j;
	  continue;
	}
	else qDebug() << "matching against action" << j.value()->label;
	
	if (j.value()->pattern.indexIn(pattern) >= 0) {
	  // Pattern matched
	  qDebug() << "found action" << j.value()->label
		   << j.value()->command
		   << "for tag" << *tag;
	  qDebug() << j.value()->pattern.capturedTexts();
	  
	  // Return action
	  return j.value();
	  
	} else {
	  qDebug() << j.value()->pattern.errorString();
	  
	}
	++j;
      }
      ++tag;
    }
    ++i;
  }
  // No action found
  return 0;
}


bool ActionManager::add(const QString &label, const QRegExp &pattern,
			const QString &command, const QStringList &tags,
			const QString &group, bool active, int priority) {
  _mutex.lock();
  Action *action = new Action;
  action->label = label;
  action->pattern = pattern;
  action->command = command;
  action->tags = tags;
  action->group = group;
  action->active = active;
  action->priority = priority;

  // Check Priority Map
  QMultiHash<QString, Action*> *actions;
  if (_actions.contains(priority)) {
    actions = _actions.value(priority);
    qDebug() << "* Priority" << priority << "has a hash of size"
	     << actions->size();
    
  } else {
    actions = new QMultiHash<QString, Action*>;
    _actions.insert(priority, actions);
    qDebug() << "* Priority" << priority << "created NEW hash";
  }
  
  foreach(QString tag, tags) {
    qDebug() << "adding for tag" << tag;
    actions->insert(tag, action);
    qDebug() << "inserted action! hash is now size of" << actions->size();
  }
  
  _mutex.unlock();
  return true;
}


bool ActionManager::loadSettings(const QHash<QString, QVariant> &hash) {

  qDebug() << "* ActionManager loading"
	   << hash.value("actions/size", 0).toInt() << "actions";

  qDebug() << hash;

  int count = 0;
  int size = hash.value("actions/size", 0).toInt();
  for (int i = 1; i <= size; i++) {
    QString prefix = "actions/" + QString::number(i);

    // Valid only if it contains these parts
    if (hash.contains(prefix+"/label") &&
	hash.contains(prefix+"/pattern") &&
	hash.contains(prefix+"/command")) {
      
      qDebug() << i << ":"
	       << hash.value(prefix+"/pattern").toString()
	       << "-->"
	       << QRegExp(hash.value(prefix+"/pattern").toString()).pattern();

      // Clean up the command (remove indentation, etc)
      QStringList commandList;
      foreach(QString line, hash.value(prefix+"/command").toString().trimmed()
	      .split("\n", QString::SkipEmptyParts)) {
	commandList << line.trimmed();
      }
      
      // Add the alias
      add(hash.value(prefix+"/label", QString::number(i)).toString(),
	  QRegExp(hash.value(prefix+"/pattern").toString()),
	  commandList.join("\n"),
	  hash.value(prefix+"/tags", "XMLNone").toStringList(),
	  hash.value(prefix+"/group", "Default").toString(),
	  hash.value(prefix+"/active", true).toBool(),
	  hash.value(prefix+"/priority", 0).toInt()
	  );
      count++;
    }
    else qDebug() << "* ActionManager unable to parse alias" << i;
    
  }

  qDebug() << "* Done loading" << count << "actions";
  return true;
}
