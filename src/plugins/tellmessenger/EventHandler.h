#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientEventHandler.h"
#include <QPointer>

class QEvent;
class SocketServer;

class EventHandler : public MClientEventHandler {
    Q_OBJECT
    
    public:
        EventHandler(AbstractPluginSession *ps, MClientPlugin *mp);
        ~EventHandler();

        void customEvent(QEvent* e);

    public slots:
        //void socketWriteData(const QByteArray &);
        void disconnectDevice();

    signals:
        void closeSocket();
        void sendToSocket(const QByteArray &);

    private:
        QPointer<SocketServer> _socketServer;
	
        void sendData(const QByteArray&);
};


#endif /* EVENTHANDLER_H */
