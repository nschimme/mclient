#include "SmartMenuBar.h"

#include "SmartMenu.h"

#include <QDebug>
#include <QMenuBar>
#include <QAction>
#include <QMenu>

SmartMenuBar::SmartMenuBar(QWidget *parent) : QMenuBar(parent) {
  qDebug() << "* SmartMenuBar created";
  _naturalPosition = 0;
}


SmartMenuBar::~SmartMenuBar() {
  foreach(QMenu *menu, _mergedMenus) {
    delete menu;
  }

  PositionSmartMap::const_iterator i = _smartMenus.constBegin();
  while (i != _smartMenus.constEnd()) {
    NameSmartHash::const_iterator j = i.value()->constBegin();
    while (j != i.value()->constEnd()) {
      PrioritySmartMap::const_iterator k = j.value()->constBegin();
      while (k != j.value()->constEnd()) {
	delete k.value();
	++k;
      }
      ++j;
    }
    ++i;
  }

  qDebug() << "* SmartMenuBar destroyed";
}


QAction* SmartMenuBar::addMenu(QMenu *menu) {
  SmartMenu *smartMenu = new SmartMenu(menu, 5, _naturalPosition++);
  addMenu(smartMenu);
  return smartMenu->menuAction();
}


QMenu* SmartMenuBar::addMenu(const QString &title) {
  SmartMenu *menu = new SmartMenu(title, 5, _naturalPosition++);
  addMenu(menu);
  return static_cast<QMenu*>(menu);
}


void SmartMenuBar::addMenu(const MenuData &menuData) {
  foreach (SmartMenu *menu, menuData)
    addMenu(menu);
}


void SmartMenuBar::addMenu(SmartMenu *menu) {
  // Check for the menu's position to get the NameSmartHash
  NameSmartHash *hash;
  if (_smartMenus.contains(menu->position()))
    hash = _smartMenus.value(menu->position());
  else {
    hash = new NameSmartHash;
    _smartMenus.insert(menu->position(), hash);
  }

  // Check for the menu's name to ge tthe PrioritySmartMap
  PrioritySmartMap *map;
  if (hash->contains(menu->title()))
    map = hash->value(menu->title());
  else {
    map = new PrioritySmartMap;
    hash->insert(menu->title(), map);
  }

  // Add the SmartMenu to the data structure
  map->insert(menu->priority(), menu);

}


void SmartMenuBar::generateMenus() {
  // Clear the current menus
  //QMenuBar::clear();

  // Step through positions first
  PositionSmartMap::const_iterator i = _smartMenus.constBegin();
  while (i != _smartMenus.constEnd()) {
    qDebug() << "looking at pos" << i.key();

    // Then go through each name
    NameSmartHash::const_iterator j = i.value()->constBegin();
    while (j != i.value()->constEnd()) {
      qDebug() << "created menu" << j.key();
      QMenu *menu = QMenuBar::addMenu(j.key());
      // TODO: If there is only one priority just add the SmartMenu?
      
      // Always start with the lowest priority menu first
      PrioritySmartMap::const_iterator k = j.value()->constBegin();
      while (k != j.value()->constEnd()) {
	qDebug() << "looking at priority" << k.key();

	// And populate the menu with actions
	foreach (QAction *action, k.value()->actions()) {
	  menu->addAction(action);
	  
	}

	++k;
      }
      ++j;
    }
    ++i;

  }
}
