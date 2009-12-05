#ifndef __TELLMESSENGER_H
#define __TELLMESSENGER_H

#include "MClientPlugin.h"

#include <QDialog>
#include <QHash>
#include <QSettings>

class TellMessengerConfig;
class EventHandler;
class QEvent;

class TellMessenger : public MClientPlugin {
    Q_OBJECT

    public:
        TellMessenger(QObject* parent=0);
        ~TellMessenger();

        // Plugin members
        void configure();
        bool startSession(PluginSession *ps);
        bool stopSession(PluginSession *ps);

	    MClientEventHandler* getEventHandler(QString s);

    private:
	    QHash<QString, QPointer<EventHandler> > _eventHandlers;

        QPointer<TellMessengerConfig> _configWidget;

};

#endif /* __TELLMESSENGER_H */
