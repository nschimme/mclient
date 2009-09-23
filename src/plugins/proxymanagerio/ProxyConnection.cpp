#include "ProxyConnection.h"
#include "ProxyServer.h"

#include <QtNetwork>

ProxyConnection::ProxyConnection(int socketDescriptor, ProxyServer *server)
  : QObject(server), _server(server) {

  // For removal from the ProxyServer's list
  connect(this, SIGNAL(disconnected(ProxyConnection *)),
	  _server, SLOT(removeProxyConnection(ProxyConnection *)));
  // For communication with the client
  connect(this, SIGNAL(authenticated(ProxyConnection *)),
	  _server, SLOT(connectProxyConnection(ProxyConnection *)));
  
  _state = PROXY_DISCONNECTED;
  _socket = new QTcpSocket(this);
  connect(_socket, SIGNAL(destroyed(QObject *)), SLOT(onSocketDestroyed()));
  connect(_socket, SIGNAL(disconnected()), SLOT(onDisconnect()));
  connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)),
	  SLOT(onError(QAbstractSocket::SocketError)));
  if (!_socket->setSocketDescriptor(socketDescriptor)) {
    // Failed to attach  socket
    onError(_socket->error());
    delete _socket;

  } else {
    // Connected!
    _peerAddress = _socket->peerAddress().toString();
    QByteArray ba("\033[1;37;41m                     \033[0m\r\n"
		  "\033[1;37;41m Welcome to mClient! \033[0m\r\n"
		  "\033[1;37;41m                     \033[0m\r\n");
    sendToSocket(ba);

    if (_server->password().isEmpty()) {
      // No password
      connect(_socket, SIGNAL(readyRead()), SLOT(onReadyRead()));
      ba = QByteArray("\r\nType \033[1m#help\033[0m for help.\r\n");
      sendToSocket(ba);
      _state = PROXY_CONNECTED;
      emit authenticated(this);
      
    } else {
      // Password
      connect(_socket, SIGNAL(readyRead()), SLOT(onPasswordRead()));
      ba = QByteArray("\r\nPassword? ");
      ba += (unsigned char) 255; // IAC
      ba += (unsigned char) 251; // WILL
      ba += (unsigned char) 1;   // OPT_ECHO
      sendToSocket(ba);
    
      // Password Timeout
      _timer = new QTimer(this);
      _timer->setSingleShot(true);
      connect(_timer, SIGNAL(timeout()), SLOT(onTimeout()));
      _timer->start(30 * 1000); // 30 seconds

      _state = PROXY_AUTHENTICATING;
    }
  }
}


ProxyConnection::~ProxyConnection() {
  if (_socket) {
    _socket->write(QByteArray("\r\n#connection closed by user.\r\n"));
    _socket->flush();
    _socket->close();
    _socket->deleteLater();
  }

  if (_timer) {
    _timer->stop();
    _timer->deleteLater();
  }

  qDebug() << "* ProxyConnection" << _peerAddress << "destroyed";
}

void ProxyConnection::onPasswordRead() {
  QByteArray ba = _socket->readAll().trimmed();
  QString password = _server->password();
  
  // Ignore telnet commands (IAC)
  if ((unsigned char) ba.at(0) == (unsigned char) 255) return ;
  
  // This is a password attempt, disconnect this slot because no
  // other attempts are allowed.
  disconnect(_socket, SIGNAL(readyRead()), this, SLOT(onPasswordRead()));

  if (ba.length() > password.length() || ba != password) {
    // Password incorrect
    sendToSocket(QString("\033[31mIncorrect password!\r\n"
			 "\r\n"
			 "Your IP (%1) has been logged.\033[0m\r\n")
		 .arg(_peerAddress)
		 .toAscii());
    _socket->flush();
    qDebug() << "* Proxy did not recognize password attempt:"
	     << ba << "from IP address"
	     << _peerAddress;

    _state = PROXY_FAILED_PASSWORD;
    _socket->disconnectFromHost();
        
  } else {
    // Password correct
    QByteArray ba("OK.\r\n\r\nType \033[1m#help\033[0m for help.\r\n");
    ba += (unsigned char) 255; // IAC
    ba += (unsigned char) 252; // WON'T
    ba += (unsigned char) 1;   // OPT_ECHO
    sendToSocket(ba);
    
    // Connect the normal slot
    connect(_socket, SIGNAL(readyRead()), SLOT(onReadyRead()));
    qDebug() << "* Proxy accepted password from IP"
	     << _socket->peerAddress().toString();
    
    _timer->stop();
    _state = PROXY_CONNECTED;
    emit authenticated(this);
  }
}


void ProxyConnection::onTimeout() {
  disconnect(_socket, SIGNAL(readyRead()), this, SLOT(onPasswordRead()));

  sendToSocket(QByteArray("Timeout.\r\n"
			  "\r\n"
			  "Please reconnect to try again.\033[0m\r\n"));
  _socket->flush();
  
  qDebug() << "* Proxy timmed out from IP address"
	   << _peerAddress;
  
  _state = PROXY_DISCONNECTED;
  _socket->disconnectFromHost();
  _socket->deleteLater();
 }


void ProxyConnection::sendToSocket(const QByteArray &ba) {
  _socket->write(ba);
}


void ProxyConnection::onReadyRead() {
  QByteArray ba = _socket->readAll(); 
  
  // Ignore telnet commands (IAC)
  if ((unsigned char) ba.at(0) == (unsigned char) 255)
    return ;
  
  emit sendUserInput(ba);
}


void ProxyConnection::onDisconnect() {
  qDebug() << "* Client disconnected from proxy";
  _socket->deleteLater();
}


void ProxyConnection::onError(QAbstractSocket::SocketError /*error*/) {
  qDebug() << "! Proxy client error:" << _socket->errorString();
  _socket->deleteLater();
}


void ProxyConnection::onSocketDestroyed() {
  emit disconnected(this);
}
