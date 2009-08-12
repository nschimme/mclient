#ifndef QTSCRIPTPLUGIN_H
#define QTSCRIPTPLUGIN_H

#include <QHash>
#include <QPointer>
#include "MClientPlugin.h"

class EventHandler;
class ScriptEngine;

class QtScriptPlugin : public MClientPlugin {
    Q_OBJECT
    
    public:
        QtScriptPlugin(QObject* parent=0);
        ~QtScriptPlugin();

        // Plugin members
        void configure();
        bool loadSettings();
        bool saveSettings() const;
        bool startSession(QString s);
        bool stopSession(QString s);

	MClientEventHandler* getEventHandler(QString s);

 private:
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
	QHash<QString, QPointer<ScriptEngine> > _scriptEngines;
	
};

#endif /* QTSCRIPTPLUGIN_H */
