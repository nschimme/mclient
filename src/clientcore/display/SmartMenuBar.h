#ifndef _SMARTMENUBAR_H
#define _SMARTMENUBAR_H

#include <QMenuBar>
#include <QMenu>
#include <QAction>

#include "MClientEventHandler.h"

class SmartMenu;

typedef QMultiMap<int, SmartMenu*> PrioritySmartMap;
typedef QHash<QString, PrioritySmartMap*> NameSmartHash;
typedef QMap<int, NameSmartHash*> PositionSmartMap;

class SmartMenuBar : public QMenuBar {
  Q_OBJECT

  public:
    SmartMenuBar(QWidget *parent=0);
    ~SmartMenuBar();

    void addMenu(const MenuData &);
    QMenu* addMenu(const QString &title);
    QAction* addMenu(QMenu *menu);
    void addMenu(SmartMenu *menu);

    void generateMenus();

 private:
    int _naturalPosition;
    QHash<QString, QMenu*> _mergedMenus;
    PositionSmartMap _smartMenus;

};

#endif /* _SMARTMENUBAR_H */
