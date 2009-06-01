#include "ScriptEngine.h"

#include "QtScriptPlugin.h"
#include "MClientEvent.h"
#include "PluginManager.h"
#include "CommandManager.h"

#include <QApplication>
#include <QDebug>

ScriptEngine *_engine;

QScriptValue print(QScriptContext *context, QScriptEngine *engine) {
  QScriptValue a = context->argument(0);

  QVariant* payload = new QVariant(a.toString());
  QStringList tags = (QStringList() << "XMLDisplayData");
  _engine->postEvent(payload, tags);

  return a;
}

QScriptValue exe(QScriptContext *context, QScriptEngine *engine) {
  QScriptValue a = context->argument(0);
  
  CommandManager::instance()->parseInput(a.toString(),
					 _engine->session());

  return a;
}

QScriptValue send(QScriptContext *context, QScriptEngine *engine) {
  QScriptValue a = context->argument(0);

  QVariant* payload = new QVariant(a.toString().append("\n"));
  QStringList tags = (QStringList() << "SendToSocketData");
  _engine->postEvent(payload, tags);

  return a;
}

QScriptValue emulate(QScriptContext *context, QScriptEngine *engine) {
  QScriptValue a = context->argument(0);

  QVariant* payload = new QVariant(a.toString());
  QStringList tags = (QStringList() << "TelnetData");
  _engine->postEvent(payload, tags);

  return a;
}

ScriptEngine::ScriptEngine(QString s, QtScriptPlugin* qsp, QObject* parent) 
    : QScriptEngine(parent) {
    _session = s;
    _qsp = qsp;
    _engine = this;

    // Connect Signals/Slots
    connect(_qsp, SIGNAL(evaluate(const QString&, const QString&)),
	    this, SLOT(evaluateExpression(const QString&, const QString&)));
    connect(this, SIGNAL(signalHandlerException(const QScriptValue&)),
	    this, SLOT(handleException(const QScriptValue&)));

    // Add C++ functions to QtScript
    QScriptValue printFunction = newFunction(print);
    globalObject().setProperty("print", printFunction);
    QScriptValue exeFunction = newFunction(exe);
    globalObject().setProperty("exe", printFunction);
    QScriptValue sendFunction = newFunction(send);
    globalObject().setProperty("send", sendFunction);
    QScriptValue emulateFunction = newFunction(emulate);
    globalObject().setProperty("emulate", emulateFunction);

    // Debugging Information
    qDebug() << "* QtScriptPlugin thread:" << _qsp->thread();
    qDebug() << "* ScriptEngine thread:" << this->thread();
}


ScriptEngine::~ScriptEngine() {
}


void ScriptEngine::evaluateExpression(const QString &expr,
				      const QString &session) {
  if (session != _session)
    return ;

  evaluate(expr);
}

void ScriptEngine::handleException(const QScriptValue &value) {
  qDebug() << "* ScriptEngine exception:" << value.toString();
}

void ScriptEngine::postEvent(QVariant *payload, const QStringList& tags) {
    MClientEvent *me = new MClientEvent(new MClientEventData(payload),
					tags,
					_session);
    QApplication::postEvent(PluginManager::instance(), me);
    //PluginManager::instance()->customEvent(me);
}
