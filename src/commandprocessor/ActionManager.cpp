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
	if (!j.value()->active) continue;
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
