#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientEventHandler.h"

class QEvent;

class EventHandler : public MClientEventHandler {
    Q_OBJECT
    
    public:
        EventHandler(QObject* parent=0);
        ~EventHandler();

        void customEvent(QEvent* e);

        // IO members
        void connectDevice();
        void disconnectDevice();

public slots:
        void socketReadData(const QByteArray&);
	void displayMessage(const QString&);
	void socketOpened();
	void socketClosed();

 signals:
	void connectToHost();
        void closeSocket();
        void sendToSocket(const QByteArray &);

 private:
	bool _openSocket;

        void sendData(const QByteArray&);

};


#endif /* EVENTHANDLER_H */
