#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H


#include <QScriptEngine>

class QtScriptPlugin;

QScriptValue print(QScriptContext *, QScriptEngine *);
QScriptValue exe(QScriptContext *, QScriptEngine *);
QScriptValue send(QScriptContext *, QScriptEngine *);
QScriptValue emulate(QScriptContext *, QScriptEngine *);

class ScriptEngine : public QScriptEngine {
    Q_OBJECT
    
    public:
        ScriptEngine(QString s, QtScriptPlugin *gsp, QObject *parent=0);
        ~ScriptEngine();

	void postEvent(QVariant *payload, const QStringList& tags);
	void parseInput(const QString &);
	void displayData(const QString&);

protected slots:
        bool evaluateExpression(const QString&, const QString&);
        bool variableCommand(const QString&, const QString&);
	void handleException(const QScriptValue &value);

    private:
        QString _session;
	QtScriptPlugin *_qsp;

 signals:
	void parseInput(const QString&, const QString&);
	void postEvent(QVariant*, const QStringList&, const QString&);

};


#endif /* SCRIPTENGINE_H */
