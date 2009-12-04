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
  
    if(_tcpServer) _tcpServer->deleteLater();
    qDebug() << "~SocketServer";
}


void SocketServer::openSocket(const int& port) {
   
    //if(port != 0) _port = port;
    qDebug() << "* binding localhost on port" << _port;
    _tcpServer->listen(QHostAddress(QHostAddress::Any), port);

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
    if (!_tcpServer) {
        //emit displayMessage("#no open connections to zap.\n");

    } else {
        //_tcpServer->close();
        _tcpServer->deleteLater();
    }

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
    _clients.append(sock);
}

void SocketServer::onDisconnect() {
  qDebug() << "Socket disconnected";
//  emit displayMessage(QString("#connection on \"%1\" closed.\n")
//		      .arg(_session));
//  emit socketClosed();
  closeSocket();
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
