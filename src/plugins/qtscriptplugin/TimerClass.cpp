#include <QtScript/QScriptClassPropertyIterator>
#include <QtScript/QScriptEngine>
#include <QDebug>

#include "TimerClass.h"
#include "TimerPrototype.h"

Q_DECLARE_METATYPE(QTimer*)
Q_DECLARE_METATYPE(TimerClass*)

TimerClass::TimerClass(QScriptEngine *engine)
: QObject(engine), QScriptClass(engine) {

  _interval = engine->toStringHandle(QLatin1String("interval"));
  _singleShot = engine->toStringHandle(QLatin1String("singleShot"));

  _proto = engine->newQObject(new TimerPrototype(this),
			      QScriptEngine::QtOwnership,
			      QScriptEngine::SkipMethodsInEnumeration
			      | QScriptEngine::ExcludeSuperClassMethods
			      | QScriptEngine::ExcludeSuperClassProperties);
  QScriptValue global = engine->globalObject();
  _proto.setPrototype(global.property("Object").property("prototype"));

  _ctor = engine->newFunction(construct, _proto);
  _ctor.setData(qScriptValueFromValue(engine, this));
}

TimerClass::~TimerClass()
{
}

QScriptClass::QueryFlags TimerClass::queryProperty(const QScriptValue &object,

						   const QScriptString &name,
						   QueryFlags flags, uint */*id*/) {
  QTimer *timer = qscriptvalue_cast<QTimer*>(object.data());
  if (!timer)
    return 0;
  if (name == _interval || name == _singleShot)
    return flags;
  return 0;
}

QScriptValue TimerClass::property(const QScriptValue &object,
				  const QScriptString &name, uint /*id*/) {
  QTimer *timer = qscriptvalue_cast<QTimer*>(object.data()); 
  if (!timer)
    return QScriptValue();
  if (name == _interval) {
    return timer->interval();
  } else if (name == _singleShot) {
    return timer->isSingleShot();
  }
  return QScriptValue();
}

void TimerClass::setProperty(QScriptValue &object,
			     const QScriptString &name,
			     uint /*id*/, const QScriptValue &value) {
  QTimer *timer = qscriptvalue_cast<QTimer*>(object.data());
  qDebug() << "Value after de-varianting setProperty: " <<  timer << name;
  if (!timer)
    return;
  if (name == _interval) {
    timer->setInterval(value.toInt32());
  } else if (name == _singleShot) {
    timer->setSingleShot(value.toBoolean());
  }
}

QScriptValue::PropertyFlags TimerClass::propertyFlags(const QScriptValue &/*object*/, const QScriptString &name, uint /*id*/) {
  if (name == _interval || name == _singleShot) {
    return QScriptValue::Undeletable | QScriptValue::SkipInEnumeration;
  }
  return QScriptValue::Undeletable;
}

QString TimerClass::name() const {
  return QLatin1String("Timer");
}

QScriptValue TimerClass::prototype() const {
  return _proto;
}

QScriptValue TimerClass::constructor() {
  return _ctor;
}

QScriptValue TimerClass::newInstance() {
  return newInstance(new QTimer(engine()));
}

QScriptValue TimerClass::newInstance(QTimer *timer) {
  QScriptValue data = engine()->newVariant(qVariantFromValue(timer));
  return engine()->newObject(this, data);
}

QScriptValue TimerClass::construct(QScriptContext *ctx, QScriptEngine *) {
  TimerClass *cls = qscriptvalue_cast<TimerClass*>(ctx->callee().data());
  if (!cls)
    return QScriptValue();
  /*
  QScriptValue arg = ctx->argument(0);
  if (arg.instanceOf(ctx->callee())) {
    return cls->newInstance(qscriptvalue_cast<QTimer*>(arg));
  }
  int size = arg.toInt32();
  qDebug() << "creating new instance" << size;
  */
  return cls->newInstance();
}
