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
        bool loadSettings();
        bool saveSettings() const;
        bool startSession(QString s);
        bool stopSession(QString s);

        // IO members
        void connectDevice();
        void disconnectDevice();
        void sendData(const QByteArray&);
        void socketReadData(const QByteArray&);

	void displayMessage(const QString&);
	void socketOpened();
	void socketClosed();


    private:
	bool _openSocket;
        QPointer<SocketReader> _socketReader;

	QHash<QString, QString> *_settings;
        QPointer<SocketManagerIOConfig> _configWidget;

 signals:
	void connectToHost();
        void sendToSocket(const QByteArray &);
        void closeSocket();        

};


#endif /* SOCKETMANAGERIO_H */
