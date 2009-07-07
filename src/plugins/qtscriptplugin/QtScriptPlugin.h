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
        bool loadSettings();
        bool saveSettings() const;
        bool startSession(QString s);
        bool stopSession(QString s);

 protected:
	void run();
	
public slots:
        void parseInput(const QString &input);

    private:
	ScriptEngine* _engine;
	
 signals:
	void evaluate(const QString&);
	void variable(const QString&);
};

#endif /* QTSCRIPTPLUGIN_H */
