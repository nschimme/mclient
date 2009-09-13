#ifndef MMAPPERPLUGIN_H
#define MMAPPERPLUGIN_H

#include "MClientPlugin.h"

#include <QHash>

class EventHandler;

class MMapperPlugin : public MClientPlugin {
    Q_OBJECT

    public:
        MMapperPlugin(QObject* parent=0);
        ~MMapperPlugin();

        // Plugin members
        void configure();
        bool startSession(PluginSession *ps);
        bool stopSession(PluginSession *ps);

	MClientEventHandler* getEventHandler(QString s);

    private:
	QHash<QString, QString> *_settings;
        QHash<QString, QPointer<EventHandler> > _eventHandlers;
};

#endif /* MMAPPERPLUGIN_H */
