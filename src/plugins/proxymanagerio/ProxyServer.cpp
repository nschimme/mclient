#include "ProxyServer.h"
#include "ProxyConnection.h"
#include "EventHandler.h"

#include <QTcpSocket>

/** Starts a local server and listens for connections */
ProxyServer::ProxyServer(EventHandler *eh)
  : QTcpServer(0), _eventHandler(eh) {

  // TODO: replace this with some configuration code
  _maxConnections = 5;

  setMaxPendingConnections(_maxConnections + 1); // Allow one more for
						 // max connection info

  connect(_eventHandler, SIGNAL(removeProxyConnection(ProxyConnection *)),
	  this, SLOT(removeProxyConnection(ProxyConnection *)));
  connect(this, SIGNAL(sendMessage(const QString &)),
	  _eventHandler, SLOT(displayMessage(const QString &)));
}


ProxyServer::~ProxyServer() {
  // Stop listening for connections
  close();

  // Delete all connections
}


void ProxyServer::start() {
  listen(QHostAddress::Any, _port);
  qDebug() << "* Created new proxy on port" << _port;
}


void ProxyServer::incomingConnection(int socketDescriptor) {
  if (_proxyConnections.size() < _maxConnections) {
    // Create a new proxy connection
    ProxyConnection *proxyConnection
      = new ProxyConnection(socketDescriptor, this);
    QDateTime current(QDateTime::currentDateTime());
    _proxyConnections[current] = proxyConnection;
    connect(proxyConnection, SIGNAL(sendMessage(const QString &)),
	    _eventHandler, SLOT(displayMessage(const QString &)));
          
  } else {
    qDebug() << "! Proxy only allows a certain number of connections";
    QTcpSocket tcpSocket;
    if (tcpSocket.setSocketDescriptor(socketDescriptor)) {
      tcpSocket.write(QByteArray("Maximum allowed connections has been reached.\r\n"));
      tcpSocket.flush();
      tcpSocket.disconnectFromHost();
      tcpSocket.deleteLater();
    }
  }
}

void ProxyServer::removeProxyConnection(ProxyConnection *proxyConnection) {
  qDebug() << "* in removeProxyConnection" << proxyConnection;
  if (proxyConnection->state() == PROXY_FAILED_PASSWORD)
    emit sendMessage("#WARNING!\r\n"
		     "#proxy client \"" + proxyConnection->peerAddress()
		     + "\" did not enter the correct password.\r\n");
  else
    emit sendMessage("#proxy client \""+ proxyConnection->peerAddress()
		     + "\" disconnected.\r\n");
  QDateTime current = _proxyConnections.key(proxyConnection);
  _proxyConnections.remove(current);
  qDebug() << _proxyConnections;
  proxyConnection->deleteLater();
}


void ProxyServer::connectProxyConnection(ProxyConnection *proxyConnection) {
  qDebug() << "* in connectProxyConnection" << proxyConnection;
  emit sendMessage("#proxy client \""+ proxyConnection->peerAddress()
		   + "\" connected.\r\n");
  
  // Input from the client is sent to mClient for processing
  connect(proxyConnection, SIGNAL(sendUserInput(const QByteArray &)),
	  _eventHandler, SLOT(sendUserInput(const QByteArray &)));
  // Display information is sent to the socket
  connect(_eventHandler, SIGNAL(sendToSocket(const QByteArray &)),
	  proxyConnection, SLOT(sendToSocket(const QByteArray &)));
}
