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
        bool loadSettings();
        bool saveSettings() const;
        bool startSession(QString s);
        bool stopSession(QString s);

	MClientEventHandler* getEventHandler(QString s);

    private:
	QHash<QString, QString> *_settings;
        QHash<QString, QPointer<EventHandler> > _eventHandlers;
};

#endif /* MMAPPERPLUGIN_H */
