#include "SocketReader.h"

#include <QMetaType>
#include <QDebug>
#include <QTcpSocket>

SocketReader::SocketReader(const QString s, QObject *parent)
  : QThread(parent), _session(s) { 
  qRegisterMetaType<QAbstractSocket::SocketError>
    ("QAbstractSocket::SocketError");
  
  _proxy.setType(QNetworkProxy::NoProxy);
}


void SocketReader::connectToHost() {
  if (!_socket) {
    emit displayMessage(QString("#trying %1:%2... ").arg(_host).arg(_port));
    run();
  }
  else if (_socket->state() < 3) {
    emit displayMessage("#already connecting... ");

  } else {
    emit displayMessage("#connection is already open. "
			"Type \033[1m#zap\033[0m to disconnect it.\r\n");

  }
  /** Old threaded code
  if (!isRunning()) start(LowPriority);
  */
}


SocketReader::~SocketReader() {
  if (isRunning()) {
    exit();
    wait();
  }
  if (_socket) _socket->deleteLater();
  qDebug() << "~SocketReader";
}


void SocketReader::sendToSocket(const QByteArray &ba) {
  if (_socket) {
    if(_socket->state() != QAbstractSocket::ConnectedState) {
      qDebug() << "Socket not connected!";
      return;
    }
    
    _socket->write(ba);
    //qDebug() << "* wrote to socket" << ba.size() << ":" << ba;
  }
}


void SocketReader::closeSocket() {
  if (!_socket) {
    emit displayMessage("#no open connections to zap.\n");

  } else {
    _socket->close();
    _socket->deleteLater();
  }

  if (isRunning()) {
    exit();
    wait();
  }
}


void SocketReader::run() {
    _socket = new QTcpSocket;
    _socket->setProxy(_proxy);

    connect(_socket, SIGNAL(connected()), SLOT(onConnect())); 
    connect(_socket, SIGNAL(disconnected()), SLOT(onDisconnect())); 
    connect(_socket, SIGNAL(readyRead()), SLOT(onReadyRead())); 
    connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
	    SLOT(onError(QAbstractSocket::SocketError))); 
    
    
    qDebug() << "* SocketReader thread:" << this->thread();
    qDebug() << "* Socket thread:" << _socket->thread();
   
    _socket->connectToHost(_host, _port);
    /*
    qDebug() << "* SocketReader entering event loop!";
    exec();
    qDebug() << "* SocketReader thread quit";
    */
}


void SocketReader::onReadyRead() {
  if (_socket) {
    QByteArray ba = _socket->readAll(); 
    emit socketReadData(ba);
  }
}


void SocketReader::onConnect() {
  emit displayMessage(QString("connected!\n"));
  emit socketOpened();
}


void SocketReader::onDisconnect() {
  qDebug() << "Socket disconnected";
  emit displayMessage(QString("#connection on \"%1\" closed.\n")
		      .arg(_session));
  emit socketClosed();
  closeSocket();
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
  emit displayMessage(errorMessage);
  if (isRunning()) {
    exit();
    wait();
  }
  closeSocket();
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
