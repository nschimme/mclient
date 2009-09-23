#ifndef PROXYCONNECTION_H
#define PROXYCONNECTION_H

#include <QPointer>
#include <QTcpSocket>
#include <QTimer>

class ProxyServer;
class EventHandler;

enum ProxyState {PROXY_AUTHENTICATING, PROXY_CONNECTED, PROXY_DISCONNECTED,
		 PROXY_FAILED_PASSWORD};

class ProxyConnection : public QObject {
  Q_OBJECT

  public:
        ProxyConnection(int, ProxyServer *);
        ~ProxyConnection();

	const QString& peerAddress() const { return _peerAddress; }
	ProxyState state() const { return _state; }

public slots:
        void sendToSocket(const QByteArray &);

private slots:
        void onDisconnect();
        void onReadyRead();
	void onPasswordRead();
        void onError(QAbstractSocket::SocketError error);   
	void onTimeout();
	void onSocketDestroyed();

 signals:
	void sendUserInput(const QByteArray &);
	void authenticated(ProxyConnection *);
	void disconnected(ProxyConnection *);

 private:
	QPointer<QTcpSocket> _socket;
	ProxyServer *_server;
	QPointer<QTimer> _timer;
	ProxyState _state;
	QString _peerAddress;

};

#endif /* PROXYCONNECTION_H */
