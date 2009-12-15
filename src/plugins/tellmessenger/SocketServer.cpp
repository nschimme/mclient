#include "SocketServer.h"

#include <QMetaType>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

SocketServer::SocketServer(const QString s, QObject *parent)
  : QThread(parent), _session(s) { 
  qRegisterMetaType<QAbstractSocket::SocketError>
    ("QAbstractSocket::SocketError");
    
    _tcpServer = new QTcpServer;

    connect(_tcpServer, SIGNAL(newConnection()), SLOT(onNewConnection()));

    connect(_tcpServer, SIGNAL(disconnected()), SLOT(onDisconnect())); 
//    connect(_socket, SIGNAL(readyRead()), SLOT(onReadyRead())); 
    connect(_tcpServer, SIGNAL(error(QAbstractSocket::SocketError)),
	    SLOT(onError(QAbstractSocket::SocketError))); 

    _port = 4244;
    openSocket();
  
}


SocketServer::~SocketServer() {
    if (isRunning()) {
        exit();
        wait();
    }
  
    closeSocket();
    if(_tcpServer) _tcpServer->deleteLater();
    qDebug() << "~SocketServer";
}


void SocketServer::openSocket() {
   
    qDebug() << "* binding localhost on port" << _port;
    _tcpServer->listen(QHostAddress(QHostAddress::Any), _port);

}


void SocketServer::sendToSocket(const QByteArray &ba) {
    if(_tcpServer) {
    
        if(_clients.count() == 0) {
            qDebug() << "No clients connected!";
            return;
        }

        QTcpSocket* cl;
        foreach(cl, _clients) {
            cl->write(ba);
            //qDebug() << "* wrote to socket" << ba.size() << ":" << ba;
        }
    }
}


void SocketServer::closeSocket() {
    if (_clients.count() != 0) {
        QTcpSocket* cl = 0;
        foreach(cl, _clients) {
            cl->deleteLater();
        }
    }

    // check the best way to exit a running thread
    if (isRunning()) {
        exit();
        wait();
    }
}


void SocketServer::run() {
    
    
//    qDebug() << "* SocketServer thread:" << this->thread();
//   qDebug() << "* Socket thread:" << _socket->thread();
   
    /*
    qDebug() << "* SocketServer entering event loop!";
    exec();
    qDebug() << "* SocketServer thread quit";
    */
}

/*
void SocketServer::onReadyRead() {
  if (_socket) {
    QByteArray ba = _socket->readAll(); 
    emit socketReadData(ba);
  }
}
*/


void SocketServer::onNewConnection() {
    QTcpSocket* sock = _tcpServer->nextPendingConnection();
    qDebug() << "* client connected to tell port";
    
    connect(sock, SIGNAL(disconnected()), this, SLOT(onClientDisconnect()));
    
    _clients.append(sock);
}

void SocketServer::onClientDisconnect() {
    qDebug() << "* client disconnected from tell port";
    // do... stuff. like get the socket pointer and delete it
    qDebug() << "  * clients remaining:" << _clients.count();
}


void SocketServer::onError(QAbstractSocket::SocketError error) {
    /*
  qWarning() << "Error involving" 
	     << _host << _port << _socket->errorString();
  
  QString errorMessage = _socket->errorString().append(".\n");
  errorMessage.replace(0, 1, errorMessage.at(0).toLower());
  */
    switch (error) {
        case QAbstractSocket::ConnectionRefusedError:
        case QAbstractSocket::HostNotFoundError:
            break;
        default:
            //errorMessage.prepend("#");
            break;
    };
//  emit displayMessage(errorMessage);


    if (isRunning()) {
        exit();
        wait();
    }
    closeSocket();
}


void SocketServer::port(const int port) {
    _port = port;
}


const int& SocketServer::port() const {
    return _port;
}


const QString& SocketServer::session() const {
    return _session;
}
