#ifndef PROXYSERVER_H
#define PROXYSERVER_H

#include <QTcpServer>

#include <QNetworkProxy>
#include <QDateTime>
#include <QMap>

class ProxyConnection;
class EventHandler;

typedef QMap<QDateTime, ProxyConnection*> ProxyConnections;

class ProxyServer : public QTcpServer {
    Q_OBJECT

    public:        
        ProxyServer(EventHandler *);
        ~ProxyServer();

	void start();

        void port(const int port) { _port = port; }
        void password(const QByteArray password) { _password = password; }

        const int& port() const { return _port; }
        const QByteArray& password() const { return _password; }
	
	const ProxyConnections& getConnections() const {
	  return _proxyConnections;
	}

public slots:
        void removeProxyConnection(ProxyConnection *);
        void connectProxyConnection(ProxyConnection *); 

 signals:
        void sendMessage(const QString &); 

 protected:
	void incomingConnection(int SocketDescriptor);

    private:
	ProxyConnections _proxyConnections;
	EventHandler *_eventHandler;
	QByteArray _password;
        int _port;
	int _maxConnections;
};

#endif /* PROXYSERVER_H */
