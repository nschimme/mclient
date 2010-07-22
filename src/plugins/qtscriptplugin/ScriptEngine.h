#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <QScriptEngine>

namespace {
  QScriptValue print(QScriptContext *, QScriptEngine *);
  QScriptValue exe(QScriptContext *, QScriptEngine *);
  QScriptValue send(QScriptContext *, QScriptEngine *);
  QScriptValue emulate(QScriptContext *, QScriptEngine *);
}

class ScriptEngine : public QScriptEngine {
    Q_OBJECT
    
    public:
        ScriptEngine(QObject *parent=0);
        ~ScriptEngine();

	void postEvent(QVariant *payload, const QStringList& tags);
	void parseInput(const QString&);
	void displayData(const QString&);

public slots:
        bool evaluateExpression(const QString&);
        bool variableCommand(const QString&);
	void handleException(const QScriptValue &value);

 signals:
	void emitParseInput(const QString&);
	void postSession(QVariant*, const QStringList&);

};


#endif /* SCRIPTENGINE_H */
