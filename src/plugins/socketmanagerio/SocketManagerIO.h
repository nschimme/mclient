#ifndef SOCKETMANAGERIO_H
#define SOCKETMANAGERIO_H

#include "MClientPlugin.h"

#include <QDialog>
#include <QHash>
#include <QSettings>

class SocketManagerIOConfig;
class EventHandler;
class QEvent;

class SocketManagerIO : public MClientPlugin {
    Q_OBJECT

    public:
        SocketManagerIO(QObject* parent=0);
        ~SocketManagerIO();

        // Plugin members
        void configure();
        bool startSession(PluginSession *ps);
        bool stopSession(PluginSession *ps);

	MClientEventHandler* getEventHandler(QString s);

    private:
	QHash<QString, QPointer<EventHandler> > _eventHandlers;

        QPointer<SocketManagerIOConfig> _configWidget;

};

#endif /* SOCKETMANAGERIO_H */
