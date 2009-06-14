#ifndef SOCKETREADER_H
#define SOCKETREADER_H

#include <QThread>
#include <QPointer>
#include <QTcpSocket>
#include <QNetworkProxy>
#include <QString>

class SocketManagerIO;
class QNetworkProxy;
class QTcpSocket;

class SocketReader : public QThread {
    Q_OBJECT

    public:        
        SocketReader(const QString&, SocketManagerIO*, QObject* parent=0);
        ~SocketReader();

        void connectToHost();
        void sendToSocket(const QByteArray* ba);
        void closeSocket();        
        
        void port(const int);
        void host(const QString);
        void proxy(const QNetworkProxy* proxy);

        const QString& session() const;
        const int& port() const;
        const QString& host() const;


 protected:
        void run();


    private:
        QPointer<QTcpSocket> _socket;
        QNetworkProxy _proxy;
        SocketManagerIO* _sm;
        QString _session;
        
        QString _host;
        int _port;
        

private slots:
        void onConnect();
        void onDisconnect();
        void onReadyRead();
        void onError(QAbstractSocket::SocketError error);
};

#endif /* SOCKETREADER_H */
