#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QThread>
#include <QPointer>
#include <QTcpSocket>
#include <QString>
#include <QVector>

class QTcpServer;

class SocketServer : public QThread {
    Q_OBJECT

    public:        
        SocketServer(const QString session, QObject* parent=0);
        ~SocketServer();

        void port(const int);
        const int& port() const;

        const QString& session() const;

    public slots:
        // Open and close a listening socket
        void openSocket();
        void closeSocket();        
        
        void sendToSocket(const QByteArray&);

    //signals:
	//    void socketReadData(const QByteArray&);

    protected:
        void run();

    private:
        QPointer<QTcpServer> _tcpServer;
        QVector<QTcpSocket*> _clients;
        
        QString _session;
        int _port;
        
    private slots:
        void onNewConnection(); 
        void onClientDisconnect();
//        void onReadyRead();
        void onError(QAbstractSocket::SocketError error);
};

#endif /* SOCKETSERVER_H */
