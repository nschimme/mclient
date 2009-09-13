#ifndef SOCKETREADER_H
#define SOCKETREADER_H

#include <QThread>
#include <QPointer>
#include <QTcpSocket>
#include <QNetworkProxy>
#include <QString>

class QNetworkProxy;
class QTcpSocket;

class SocketReader : public QThread {
    Q_OBJECT

    public:        
        SocketReader(const QString session, QObject* parent=0);
        ~SocketReader();

        void port(const int);
        void host(const QString);
        void proxy(const QNetworkProxy* proxy);

        const QString& session() const;
        const int& port() const;
        const QString& host() const;


public slots:
        void connectToHost();
        void sendToSocket(const QByteArray &);
        void closeSocket();        

 signals:
	void socketReadData(const QByteArray &);
	void displayMessage(const QString &);
	void socketOpened();
	void socketClosed();

 protected:
        void run();


    private:
        QPointer<QTcpSocket> _socket;
        QNetworkProxy _proxy;
        
        QString _host, _session;
        int _port;
        

private slots:
        void onConnect();
        void onDisconnect();
        void onReadyRead();
        void onError(QAbstractSocket::SocketError error);
};

#endif /* SOCKETREADER_H */
