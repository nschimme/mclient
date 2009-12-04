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

        const QString& session() const;
        const int& port() const;

    public slots:
        void openSocket(const int& port=0);
        void sendToSocket(const QByteArray&);
        void closeSocket();        

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
        void onDisconnect();
//        void onReadyRead();
        void onError(QAbstractSocket::SocketError error);
};

#endif /* SOCKETSERVER_H */
