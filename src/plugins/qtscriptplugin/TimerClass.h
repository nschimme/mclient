#ifndef TIMERCLASS_H
#define TIMERCLASS_H

#include <QTimer>
#include <QtCore/QObject>
#include <QtScript/QScriptClass>
#include <QtScript/QScriptString>

class TimerClass : public QObject, public QScriptClass {
 public:
     TimerClass(QScriptEngine *engine);
     ~TimerClass();

     QScriptValue constructor();

     QScriptValue newInstance();
     QScriptValue newInstance(QTimer *timer);

     QueryFlags queryProperty(const QScriptValue &object,
                              const QScriptString &name,
                              QueryFlags flags, uint *id);

     QScriptValue property(const QScriptValue &object,
                           const QScriptString &name, uint id);

     void setProperty(QScriptValue &object, const QScriptString &name,
                      uint id, const QScriptValue &value);

     QScriptValue::PropertyFlags propertyFlags(const QScriptValue &object, const QScriptString &name, uint id);

     QString name() const;

     QScriptValue prototype() const;

 private:
     static QScriptValue construct(QScriptContext *ctx, QScriptEngine *eng);

     QScriptString _interval, _singleShot;
     QScriptValue _proto;
     QScriptValue _ctor;
};

#endif
