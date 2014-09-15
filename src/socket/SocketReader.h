#ifndef SOCKETREADER_H
#define SOCKETREADER_H

#include <QObject>
#include <QPointer>
#include <QTcpSocket>
#include <QNetworkProxy>
#include <QString>

class Session;
class QNetworkProxy;
class QTcpSocket;

class SocketReader : public QObject {
    Q_OBJECT

public:
    SocketReader(Session *session);
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

private:
    QPointer<QTcpSocket> _socket;
    QNetworkProxy _proxy;

    QString _host, _session;
    int _port;

    void createSocket();

private slots:
    void onConnect();
    void onDisconnect();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError error);
};

#endif /* SOCKETREADER_H */
