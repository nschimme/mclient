#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientEventHandler.h"

class QEvent;
class ProxyServer;
class ProxyConnection;

class EventHandler : public MClientEventHandler {
    Q_OBJECT
    
    public:
         EventHandler(AbstractPluginSession *ps, MClientPlugin *mp);
        ~EventHandler();

        void customEvent(QEvent* e);

public slots:
        void sendUserInput(const QByteArray &);
        void displayMessage(const QString &);

 signals:
        void sendToSocket(const QByteArray &);
	void removeProxyConnection(ProxyConnection *);
	
 private:
	ProxyServer *_proxyServer;

	void echoMode(bool);
	bool proxyCommand(const QString &);
       
};


#endif /* EVENTHANDLER_H */
