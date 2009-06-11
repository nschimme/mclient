#include "SocketReader.h"
#include "SocketManagerIO.h"

#include <QMetaType>
#include <QDebug>
#include <QTcpSocket>

SocketReader::SocketReader(QString s, SocketManagerIO *sm, QObject *parent) 
    : QThread(parent) { 
  qRegisterMetaType<QAbstractSocket::SocketError>
    ("QAbstractSocket::SocketError");
   
    _session = s;
    _sm = sm;

    _proxy.setType(QNetworkProxy::NoProxy);
}


void SocketReader::connectToHost() {
  _sm->displayMessage(QString("#trying %1:%2... ").arg(_host).arg(_port),
		      _session);
  
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
	    SLOT(onError())); 

    qDebug() << "* SocketManagerIO thread:" << _sm->thread();
    qDebug() << "* SocketReader thread:" << this->thread();
    qDebug() << "* Socket thread:" << _socket->thread();
   
    _socket->connectToHost(_host, _port);

    _socket->waitForConnected(5000);
    exec();
}


void SocketReader::onReadyRead() {
    QByteArray ba = _socket->readAll(); 
    //qDebug() << _socket->thread() << _session << "reading data:" << ba;
    _sm->socketReadData(ba, _session);
}


void SocketReader::onConnect() {
    _sm->displayMessage(QString("connected!\n"), _session);
    _sm->socketOpened(this);
}


void SocketReader::onDisconnect() {
    _sm->displayMessage(QString("#connection on \"%1\" closed.\n").arg(_session), _session);
    _sm->socketClosed(this);
}


void SocketReader::onError() {
    qWarning() << "Error involving" 
       << _host << _port << _socket->errorString();
    _sm->displayMessage(_socket->errorString().append(".\n"), _session);
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
