#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientEventHandler.h"
#include <QPointer>

class QEvent;
class SocketReader;

class EventHandler : public MClientEventHandler {
    Q_OBJECT
    
    public:
        EventHandler(AbstractPluginSession *ps, MClientPlugin *mp);
        ~EventHandler();

        void customEvent(QEvent* e);

	const MenuData& createMenus();

public slots:
        void socketReadData(const QByteArray &);
	void displayMessage(const QString &);
	void socketOpened();
	void socketClosed();

        // Menu/Action members
        void connectDevice(const QString & =0);
        void disconnectDevice();

 signals:
	void connectToHost();
        void closeSocket();
        void sendToSocket(const QByteArray &);

 private:
	bool _autoConnect;
        QPointer<SocketReader> _socketReader;
	
        void sendData(const QByteArray&);
	void sendMUMEGARequest();
};


#endif /* EVENTHANDLER_H */
