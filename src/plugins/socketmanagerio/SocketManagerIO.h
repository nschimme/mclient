#ifndef SOCKETMANAGERIO_H
#define SOCKETMANAGERIO_H

#include "MClientPlugin.h"

#include <QDialog>
#include <QHash>
#include <QSettings>

class SocketManagerIOConfig;
class SocketReader;
class EventHandler;

class QByteArray;
class QEvent;
class QString;

class SocketManagerIO : public MClientPlugin {
    Q_OBJECT

    public:
        SocketManagerIO(QObject* parent=0);
        ~SocketManagerIO();

        // Plugin members
        void configure();
        bool loadSettings();
        bool saveSettings() const;
        bool startSession(QString s);
        bool stopSession(QString s);

	MClientEventHandler* getEventHandler(QString s);

    private:
        QHash<QString, QPointer<SocketReader> > _socketReaders;
	QHash<QString, QPointer<EventHandler> > _eventHandlers;

	QHash<QString, QString> *_settings;
        QPointer<SocketManagerIOConfig> _configWidget;

};


#endif /* SOCKETMANAGERIO_H */
