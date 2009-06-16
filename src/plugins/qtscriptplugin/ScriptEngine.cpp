#include "ScriptEngine.h"

#include "QtScriptPlugin.h"

#include <QApplication>
#include <QDebug>
#include <QScriptValueIterator>

QScriptValue print(QScriptContext *context, QScriptEngine *engine) {
  QScriptValue a = context->argument(0);

  ScriptEngine *e = qobject_cast<ScriptEngine*>(engine);
  e->displayData(a.toString());

  return a;
}

QScriptValue exe(QScriptContext *context, QScriptEngine *engine) {
  QScriptValue a = context->argument(0);
  
  ScriptEngine *e = qobject_cast<ScriptEngine*>(engine);
  e->parseInput(a.toString());

  return a;
}

QScriptValue send(QScriptContext *context, QScriptEngine *engine) {
  QScriptValue a = context->argument(0);

  QVariant* payload = new QVariant(a.toString().append("\n"));
  QStringList tags = (QStringList() << "SendToSocketData");
  ScriptEngine *e = qobject_cast<ScriptEngine*>(engine);
  e->postEvent(payload, tags);

  return a;
}

QScriptValue emulate(QScriptContext *context, QScriptEngine *engine) {
  QScriptValue a = context->argument(0);

  QVariant* payload = new QVariant(a.toString());
  QStringList tags = (QStringList() << "TelnetData");
  ScriptEngine *e = qobject_cast<ScriptEngine*>(engine);
  e->postEvent(payload, tags);

  return a;
}

ScriptEngine::ScriptEngine(QString s, QtScriptPlugin* qsp, QObject* parent) 
  : QScriptEngine(parent) {
  _session = s;
  _qsp = qsp;
  
  // Connect Signals/Slots
  connect(_qsp, SIGNAL(evaluate(const QString&)),
	  this, SLOT(evaluateExpression(const QString&)));
  connect(_qsp, SIGNAL(variable(const QString&)),
	  this, SLOT(variableCommand(const QString&)));
  connect(this, SIGNAL(signalHandlerException(const QScriptValue&)),
	  this, SLOT(handleException(const QScriptValue&)));
  connect(this, SIGNAL(emitParseInput(const QString&)),
	  _qsp, SLOT(parseInput(const QString&)));
  connect(this, SIGNAL(postSession(QVariant*, const QStringList&)),
	  _qsp, SLOT(postSession(QVariant*, const QStringList&)));
  
  
  // Add C++ functions to QtScript
  QFlags<QScriptValue::PropertyFlag> functionFlags(QScriptValue::ReadOnly | QScriptValue::SkipInEnumeration);
  QScriptValue printFunction = newFunction(print);
  globalObject().setProperty("print", printFunction, functionFlags);
  QScriptValue exeFunction = newFunction(exe);
  globalObject().setProperty("exe", exeFunction, functionFlags);
  QScriptValue sendFunction = newFunction(send);
  globalObject().setProperty("send", sendFunction, functionFlags);
  QScriptValue emulateFunction = newFunction(emulate);
  globalObject().setProperty("emulate", emulateFunction, functionFlags);
  
  // Debugging Information
  qDebug() << "* QtScriptPlugin thread:" << _qsp->thread();
  qDebug() << "* ScriptEngine thread:" << this->thread();
}


ScriptEngine::~ScriptEngine() {
}


bool ScriptEngine::evaluateExpression(const QString &expr) {
  QScriptValue result = evaluate(expr);

  // Detect if an exception occured
  if (hasUncaughtException()) {
    QStringList output;
    output << "#" << uncaughtException().toString()
	   << " on line " << QString("%1").arg(uncaughtExceptionLineNumber())
	   << " with backstrace " << uncaughtExceptionBacktrace()
	   << "\n";
    displayData(output.join(""));
    return false;
  }

  return true;
}

bool ScriptEngine::variableCommand(const QString &arguments) {
  // Create a Map of variables
  QMap<QString, QScriptValue> variables;
  QScriptValueIterator it(globalObject());
  while (it.hasNext()) {
    it.next();
    if (it.flags() & QScriptValue::SkipInEnumeration) continue;
    variables.insert(it.name(), it.value());
  }

  if (arguments.isEmpty()) {
    // Display variables
    QStringList output;
    QMap<QString, QScriptValue>::const_iterator i = variables.constBegin();
    while (i != variables.constEnd()) {
      output << "" + i.key() + " = " + i.value().toString() + "\n";
      ++i;
    }
    if (output.isEmpty())
      output.prepend("#no variables are defined.\n");
    else
      output.prepend(QString("#the following variable%1 defined:\n").arg(output.size()==1?" is":"s are"));
    
    displayData(output.join(""));
    return true;
  }

  QRegExp variableRx("^([^\\s=]+)(\\s*=?\\s*)(.*)");
  if (!variableRx.exactMatch(arguments)) {
    // Incorrect command syntax
    qDebug() << "* Unknown syntax in Variable' command regular expression"
	     << variableRx.errorString();
    return false;
  }
  // Parse the command
  QStringList cmd = variableRx.capturedTexts();
  qDebug() << cmd;

  QString name(cmd.at(1));
  QString symbol(cmd.at(2));
  QString value(cmd.at(3));

  if (symbol.isEmpty()) {

    if (variables.contains(name)) {
      // Display variable
      displayData(QString("%1 = %2\n").arg(name,
					   variables[name].toString()));

    } else {
      // Error, no variable exists
      displayData("#unknown variable, cannot show: \""+name+"\"\n");
      return false;
    }

  }
  else if (value.isEmpty()) {
    if (variables.contains(name)) {
      // Delete variable
      variables.remove(name);
      displayData("#deleted variable: \""+name+"\"\n");

    } else {
      // Error, no variable exists
      displayData("#unknown variable, cannot delete: \""+name+"\"\n");
      return false;

    }
  } else if (symbol.trimmed() == "=") {
    
    QString output;
    if (variables.contains(name))
      output = "#variable \""+name+"\" changed\n";
    else 
      output = "#new variable \""+name+"\"\n";

    // Create variable
    QString command = QString("var %1 = %2;").arg(name, value);
    if (evaluateExpression(command))
      displayData(output);

  }
  
  return true;
}

void ScriptEngine::handleException(const QScriptValue &value) {
  qDebug() << "* ScriptEngine exception:" << value.toString();
}

void ScriptEngine::postEvent(QVariant *payload, const QStringList& tags) {
  emit postSession(payload, tags);
}

void ScriptEngine::parseInput(const QString &text) {
  emit emitParseInput(text);
}

void ScriptEngine::displayData(const QString& text) {
  QVariant *payload = new QVariant(text);
  QStringList tags("DisplayData");  
  emit postSession(payload, tags);
}
