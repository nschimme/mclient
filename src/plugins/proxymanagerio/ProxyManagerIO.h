#ifndef PROXYMANAGERIO_H
#define PROXYMANAGERIO_H

#include "MClientPlugin.h"

#include <QDialog>
#include <QHash>
#include <QSettings>

class ProxyServer;
class EventHandler;

class ProxyManagerIO : public MClientPlugin {
    Q_OBJECT

    public:
        ProxyManagerIO(QObject* parent=0);
        ~ProxyManagerIO();

        // Plugin members
        void configure();
        bool startSession(PluginSession *ps);
        bool stopSession(PluginSession *ps);

	MClientEventHandler* getEventHandler(QString s);

    private:
        QHash<QString, QPointer<ProxyServer> > _proxyServers;
	QHash<QString, QPointer<EventHandler> > _eventHandlers;

	QHash<QString, QString> *_settings;
};


#endif /* PROXYMANAGERIO_H */
