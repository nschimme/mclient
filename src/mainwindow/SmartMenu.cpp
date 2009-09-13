#include "SmartMenu.h"

#include <QDebug>
#include <QMenu>

SmartMenu::SmartMenu(int pr, int pos, QWidget *parent)
  : QMenu(parent), _priority(pr), _position(pos) {
  qDebug() << "* SmartMenu created" << pr << pos;
}


SmartMenu::SmartMenu(const QString &t, int pr, int pos, QWidget *parent)
  : QMenu(t, parent), _priority(pr), _position(pos) {
  qDebug() << "* SmartMenu created" << t << pr << pos;
}


SmartMenu::SmartMenu(QMenu *menu, int pr, int pos, QWidget *parent)
  : QMenu(parent), _priority(pr), _position(pos) {
  foreach (QAction *action, menu->actions())
    QMenu::addAction(action);
}


SmartMenu::~SmartMenu() {
  foreach (QAction *action, actions())
    delete action;
  qDebug() << "* SmartMenu" << title() << "destroyed";
}
