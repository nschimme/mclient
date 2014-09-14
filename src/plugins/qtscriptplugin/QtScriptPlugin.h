#ifndef QTSCRIPTPLUGIN_H
#define QTSCRIPTPLUGIN_H

#include <QHash>
#include <QPointer>
#include "MClientPlugin.h"

class AbstractPluginSession;
class EventHandler;

class QtScriptPlugin : public MClientPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MClientPluginInterface_iid FILE "qtscriptplugin.json")

    public:
        QtScriptPlugin(QObject* parent=0);
        ~QtScriptPlugin();

        // Plugin members
        void configure();
        bool startSession(AbstractPluginSession *ps);
        bool stopSession(const QString &s);

	MClientEventHandler* getEventHandler(const QString &s);

 private:
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
	
};

#endif /* QTSCRIPTPLUGIN_H */
