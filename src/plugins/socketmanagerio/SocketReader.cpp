#include "SocketReader.h"
#include "SocketManagerIO.h"

#include <QMetaType>
#include <QDebug>
#include <QTcpSocket>

SocketReader::SocketReader(const QString &s, SocketManagerIO *sm,
			   QObject *parent) 
    : QThread(parent) { 
  qRegisterMetaType<QAbstractSocket::SocketError>
    ("QAbstractSocket::SocketError");

    _session = s;
    _sm = sm;

    _proxy.setType(QNetworkProxy::NoProxy);
}


void SocketReader::connectToHost() {
  _sm->displayMessage(QString("#trying %1:%2... ").arg(_host).arg(_port));
  
  if(!isRunning()) start(LowPriority);
}


SocketReader::~SocketReader() {
    exit();
    wait();
    delete _socket;
}


void SocketReader::sendToSocket(const QByteArray* ba) {
    if(_socket->state() != QAbstractSocket::ConnectedState) return;
    _socket->write(ba->data());
    //qDebug() << dataLength << "bytes written";
    delete ba;
}


void SocketReader::closeSocket() {
    _socket->close();
    exit();
    wait();
    delete _socket;
}


void SocketReader::run() {
    _socket = new QTcpSocket;
    _socket->setProxy(_proxy);
    
    connect(_socket, SIGNAL(connected()), SLOT(onConnect())); 
    connect(_socket, SIGNAL(disconnected()), SLOT(onDisconnect())); 
    connect(_socket, SIGNAL(readyRead()), SLOT(onReadyRead())); 
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
	    this, SLOT(onError(QAbstractSocket::SocketError))); 

    qDebug() << "* SocketManagerIO thread:" << _sm->thread();
    qDebug() << "* SocketReader thread:" << this->thread();
    qDebug() << "* Socket thread:" << _socket->thread();
   
    _socket->connectToHost(_host, _port);

    _socket->waitForConnected(5000);
    exec();
}


void SocketReader::onReadyRead() {
    QByteArray ba = _socket->readAll(); 
    _sm->socketReadData(ba);
}


void SocketReader::onConnect() {
    _sm->displayMessage(QString("connected!\n"));
    _sm->socketOpened();
}


void SocketReader::onDisconnect() {
    _sm->displayMessage(QString("#connection on \"%1\" closed.\n")
			.arg(_session));
    _sm->socketClosed();
}


void SocketReader::onError(QAbstractSocket::SocketError error) {
    qWarning() << "Error involving" 
	       << _host << _port << _socket->errorString();

    QString errorMessage = _socket->errorString().append(".\n");
    errorMessage.replace(0, 1, errorMessage.at(0).toLower());
    switch (error) {
    case QAbstractSocket::ConnectionRefusedError:
    case QAbstractSocket::HostNotFoundError:
      break;
    default:
      errorMessage.prepend("#");
      break;
    };
    _sm->displayMessage(errorMessage);
    exit();
    wait();
    delete _socket;
}


void SocketReader::host(const QString host) {
    _host = host;
}


void SocketReader::port(const int port) {
    _port = port;
}


void SocketReader::proxy(const QNetworkProxy* proxy) {
    _proxy = *proxy;
    if (_socket) _socket->setProxy(_proxy);

}


const int& SocketReader::port() const {
    return _port;
}


const QString& SocketReader::session() const {
    return _session;
}
