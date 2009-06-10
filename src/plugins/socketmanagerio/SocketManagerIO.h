#ifndef SOCKETMANAGERIO_H
#define SOCKETMANAGERIO_H

#include "MClientIOPlugin.h"

#include <QDialog>
#include <QMultiHash>
#include <QPair>
#include <QSettings>

class SocketManagerIOConfig;
class SocketReader;

class QByteArray;
class QEvent;
class QString;
class QVariant;


class SocketManagerIO : public MClientIOPlugin {
    Q_OBJECT

    public:
        SocketManagerIO(QObject* parent=0);
        ~SocketManagerIO();

        // Plugin members
        void customEvent(QEvent* e);
        void configure();
        const bool loadSettings();
        const bool saveSettings() const;
        const bool startSession(QString s);
        const bool stopSession(QString s);

        // IO members
        void connectDevice(QString s);
        void disconnectDevice(QString s);
        void sendData(const QByteArray&, const QString&);
        void socketReadData(const QByteArray&, const QString&);

	void displayMessage(const QString&, const QString&);
	void socketOpened(SocketReader*);
	void socketClosed(SocketReader*);


    private:
	QHash<QString, QString> *_settings;
        QHash<QString, SocketReader*> _socketReaders, _openSockets;
        QPointer<SocketManagerIOConfig> _configWidget;
};


#endif /* SOCKETMANAGERIO_H */
