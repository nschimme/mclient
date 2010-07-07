#ifndef _SMARTTABWIDGET_H
#define _SMARTTABWIDGET_H

#include <QTabWidget>

class SmartTabWidget : public QTabWidget {
  Q_OBJECT

  public:
    SmartTabWidget(QWidget *parent=0);
    virtual ~SmartTabWidget();

 protected:
    // Reimplemented
    void tabInserted(int index);
    void tabRemoved(int index);
};

#endif /* _SMARTTABWIDGET_H */
