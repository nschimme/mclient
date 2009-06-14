#ifndef QTSCRIPTPLUGIN_H
#define QTSCRIPTPLUGIN_H

#include "MClientPlugin.h"

#include <QHash>

class QEvent;
class ScriptEngine;

class QtScriptPlugin : public MClientPlugin {
    Q_OBJECT
    
    public:
        QtScriptPlugin(QObject* parent=0);
        ~QtScriptPlugin();

        // Plugin members
        void customEvent(QEvent* e);
        void configure();
        const bool loadSettings();
        const bool saveSettings() const;
        const bool startSession(QString s);
        const bool stopSession(QString s);
	
public slots:
        void parseInput(const QString &input, const QString &session);

    private:
	QHash<QString, ScriptEngine*> _engines;
	
 signals:
	void evaluate(const QString&, const QString&);
	void variable(const QString&, const QString&);
};

#endif /* QTSCRIPTPLUGIN_H */
