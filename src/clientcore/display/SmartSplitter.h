#ifndef _SMARTSPLITTER_H
#define _SMARTSPLITTER_H

#include <QSplitter>

class SmartSplitter : public QSplitter {
  Q_OBJECT

  public:
    SmartSplitter(QWidget *parent=0);
    SmartSplitter(Qt::Orientation, QWidget *parent=0);
    virtual ~SmartSplitter();

    // Reimplemented
    void addWidget(QWidget *widget);
    void insertWidget(int index, QWidget *widget);

public slots:
    void resizeWidget(QWidget *widget);

 signals:
    void resizedWidget(); 
};

#endif /* _SMARTSPLITTER_H */
