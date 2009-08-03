#ifndef _OBJECTVIEWER_H
#define _OBJECTVIEWER_H

#include <QWidget>

class AliasManager;
class ObjectView;

class ObjectViewer : public QWidget {
  Q_OBJECT
    
  public:
    ObjectViewer(AliasManager *mgr, QWidget *parent=0);
    virtual ~ObjectViewer();

    QSize sizeHint() const;

   private:
    AliasManager *_mgr;
};

#endif /* _OBJECTVIEWER_H */
