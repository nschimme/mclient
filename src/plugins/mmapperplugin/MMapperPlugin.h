#ifndef MMAPPERPLUGIN_H
#define MMAPPERPLUGIN_H

#include "MClientPlugin.h"

#include <QHash>

class EventHandler;

class MMapperPlugin : public MClientPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID MClientPluginInterface_iid FILE "mmapperplugin.json")

    public:
        MMapperPlugin(QObject* parent=0);
        ~MMapperPlugin();

        // Plugin members
        void configure();
        bool startSession(AbstractPluginSession *ps);
        bool stopSession(const QString &session);

	MClientEventHandler* getEventHandler(const QString &s);

    private:
	QHash<QString, QString> *_settings;
        QHash<QString, QPointer<EventHandler> > _eventHandlers;
};

#endif /* MMAPPERPLUGIN_H */
