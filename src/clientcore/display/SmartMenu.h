#ifndef _SMARTMENU_H
#define _SMARTMENU_H

#include <QMenu>

class SmartMenu : public QMenu {
  Q_OBJECT

  public:
    SmartMenu(int priority =5, int position =5, QWidget *parent=0);
    SmartMenu(const QString &title, int priority =5, int position =5, QWidget *parent=0);
    SmartMenu(QMenu *menu, int priority =5, int position =5, QWidget *parent=0);
    ~SmartMenu();

    int priority() const { return _priority; }
    int position() const { return _position; }

 private:
    int _priority, _position;
};

#endif /* _SMARTMENU_H */
