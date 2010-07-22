#include "TimerPrototype.h"

#include <QtScript/QScriptEngine>
#include <QDebug>

Q_DECLARE_METATYPE(QTimer*)

TimerPrototype::TimerPrototype(QObject *parent)
: QObject(parent) {
}

TimerPrototype::~TimerPrototype() {}

QTimer *TimerPrototype::thisTimer() const {
  return qscriptvalue_cast<QTimer*>(thisObject().data());
}

void TimerPrototype::start() const {
  QTimer *timer = thisTimer();
  if (timer) timer->start();
}

void TimerPrototype::stop() const {
  QTimer *timer = thisTimer();
  if (timer) timer->stop();
}

void TimerPrototype::start(int interval, const QScriptValue &receiver, const QScriptValue &function) const {
  QTimer *timer = thisTimer();
  if (timer) {
    qScriptConnect(timer, SIGNAL(timeout()), receiver, function);
    timer->start(interval);
  }
}

bool TimerPrototype::isActive() const {
  QTimer *timer = thisTimer();
  if (timer) return timer->isActive();
  return false;
}

QScriptValue TimerPrototype::valueOf() const {
  return thisObject().data();
}
