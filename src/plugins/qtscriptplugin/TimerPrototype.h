#ifndef TIMERPROTOTYPE_H
#define TIMERPROTOTYPE_H

#include <QTimer>
#include <QtCore/QObject>
#include <QtScript/QScriptable>
#include <QtScript/QScriptValue>

class TimerPrototype : public QObject, public QScriptable {
  Q_OBJECT
    public:
  TimerPrototype(QObject *parent = 0);
  ~TimerPrototype();

public slots:
  void start(int interval, const QScriptValue &receiver, const QScriptValue &function) const;
  void start() const;
  void stop() const;
  bool isActive() const;
  QScriptValue valueOf() const;

 private:
  QTimer *thisTimer() const;
};

#endif
