#ifndef MMAPPERPLUGIN_H
#define MMAPPERPLUGIN_H

#include "MClientDisplayPlugin.h"

#include <QHash>

class EventHandler;
class MapperManager;

class MMapperPlugin : public MClientDisplayPlugin {
    Q_OBJECT

    public:
        MMapperPlugin(QWidget* parent=0);
        ~MMapperPlugin();

        // Plugin members
        void configure();
        bool loadSettings();
        bool saveSettings() const;
        bool startSession(QString s);
        bool stopSession(QString s);

        // Display members
        bool initDisplay(QString s);
        QWidget* getWidget(QString s);

	MClientEventHandler* getEventHandler(QString s);

    private:
	QHash<QString, QString> *_settings;
        QHash<QString, QPointer<MapperManager> > _mappers;
        QHash<QString, QPointer<EventHandler> > _eventHandlers;
};

#endif /* MMAPPERPLUGIN_H */
