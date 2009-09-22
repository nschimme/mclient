#ifndef QTSCRIPTPLUGIN_H
#define QTSCRIPTPLUGIN_H

#include <QHash>
#include <QPointer>
#include "MClientPlugin.h"

class PluginSession;
class EventHandler;

class QtScriptPlugin : public MClientPlugin {
    Q_OBJECT
    
    public:
        QtScriptPlugin(QObject* parent=0);
        ~QtScriptPlugin();

        // Plugin members
        void configure();
        bool startSession(PluginSession *ps);
        bool stopSession(PluginSession *ps);

	MClientEventHandler* getEventHandler(QString s);

 private:
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
	
};

#endif /* QTSCRIPTPLUGIN_H */
