#ifndef SOCKETREADER_H
#define SOCKETREADER_H

#include <QThread>
#include <QTcpSocket>
#include <QNetworkProxy>
#include <QString>

class SocketManagerIO;
class QNetworkProxy;
class QTcpSocket;

class SocketReader : public QThread {
    Q_OBJECT

    public:
        
        SocketReader(QString s, SocketManagerIO* sm, QObject* parent=0);
        ~SocketReader();

        void connectToHost();
        
        const int& port() const;
        void port(const int);
        const QString& host() const;
        void host(const QString);
        void proxy(const QNetworkProxy* proxy);

        void closeSocket() const;
        
        void sendToSocket(const QByteArray* ba);

        void run();

        const QString& session() const;


    private:
        QTcpSocket* _socket;
        QNetworkProxy _proxy;
        SocketManagerIO* _sm;
        QString _session;
        
        // Config
        QString _host;
        int _port;
        

    private slots:
        void on_connect();
        void on_disconnect();
        void on_readyRead();
        void on_error();
};


//By Runner
#define ntohll(x) (((int64_t)(ntohl((int)((x << 32) >> 32))) << 32) | (unsigned int)ntohl(((int)(x >> 32))))
#define htonll(x) ntohll(x)

#endif /* SOCKETREADER_H */
