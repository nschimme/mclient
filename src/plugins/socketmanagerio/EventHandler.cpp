#include "EventHandler.h"
#include "SocketReader.h"

#include <QDebug>
#include <QEvent>
#include <QVariant>

#include "SmartMenu.h"
#include <QAction>

#include "MClientEvent.h"
#include "AbstractPluginSession.h"
#include "ConfigEntry.h"

EventHandler::EventHandler(AbstractPluginSession *ps, MClientPlugin *mp)
  : MClientEventHandler(ps, mp) {
  _socketReader = new SocketReader(ps->session(), this);

  QString cfg = QString("config/");
  
  // Host settings
  QString host = _config->value(cfg+"connection/host", "mume.org").toString();
  int port = _config->value(cfg+"connection/port", "23").toInt();
  _autoConnect = _config->value(cfg+"connection/autoconnect", true).toBool();
  
  // Proxy settings
  QString proxy_host = _config->value(cfg+"proxy/host",
				      "proxy.example.com").toString();
  int proxy_port = _config->value(cfg+"proxy/port", "0").toInt();
  QString proxy_user = _config->value(cfg+"proxy/proxy_user",
				      "").toString();
  QString proxy_pass = _config->value(cfg+"proxy/proxy_pass",
				      "").toString();
  
  if(proxy_port != 0 && !proxy_host.isEmpty()) {
    QNetworkProxy* proxy = new QNetworkProxy();
    //proxy->setType(QNetworkProxy::Socks5Proxy);
    proxy->setHostName(proxy_host);
    proxy->setPort(proxy_port);
    proxy->setUser(proxy_user);
    proxy->setPassword(proxy_pass);
      
    _socketReader->proxy(proxy);
    qDebug() << "* added proxy" << proxy_host << proxy_port
	     << "to SocketReader";
  }
  _socketReader->host(host);
  _socketReader->port(port);
    
  // Signals and slots
  connect(this, SIGNAL(connectToHost()), _socketReader, SLOT(connectToHost()));
  connect(this, SIGNAL(closeSocket()), _socketReader, SLOT(closeSocket()));
  connect(this, SIGNAL(sendToSocket(const QByteArray &)),
	  _socketReader, SLOT(sendToSocket(const QByteArray &)));
    
  connect(_socketReader, SIGNAL(socketReadData(const QByteArray &)),
	  SLOT(socketReadData(const QByteArray &)));
  connect(_socketReader, SIGNAL(displayMessage(const QString &)),
	  SLOT(displayMessage(const QString &)));
  connect(_socketReader, SIGNAL(socketOpened()),
	  SLOT(socketOpened()));
  connect(_socketReader, SIGNAL(socketClosed()),
	  SLOT(socketClosed()));
  
}


EventHandler::~EventHandler() {
  _socketReader->disconnect();
  _socketReader->deleteLater();
}


void EventHandler::customEvent(QEvent *e) {
  if (e->type() == 10001) {    

    // Forward the event to the next in the chain
    forwardEvent(e);

    MClientEvent* me = static_cast<MClientEvent*>(e);

    if(me->dataTypes().contains("SendToSocketData")) {
      QByteArray ba = me->payload()->toByteArray();
      sendData(ba);
      
    } else if (me->dataTypes().contains("ConnectToHost")) {
      connectDevice(me->payload()->toString());
      
    } else if (me->dataTypes().contains("DisconnectFromHost")) {
      //QString arg = me->payload()->toString();
      disconnectDevice();
      
    } else if (me->dataTypes().contains("MUMEPromptGARequest")) {
      //do not allow server to suppress go-aheads (MUME is backwards)
      sendMUMEGARequest();
      
    } else if (me->dataTypes().contains("DoneLoading")) {
      // auto connect on session start
      if (_autoConnect) connectDevice();

    }

    
  }
  else 
    qDebug() << "SocketManagerIO got a customEvent of type" << e->type();
}


// Menu
const MenuData& EventHandler::createMenus() {
  QAction *connectAct = new QAction(tr("&Connect"), 0);
  connectAct->setStatusTip(tr("Connect to the remote host"));
  connect(connectAct, SIGNAL(triggered()), SLOT(connectDevice()) );
  
  QAction *disconnectAct = new QAction(tr("&Disconnect"), 0);
  disconnectAct->setStatusTip(tr("Disconnect from the host"));
  connect(disconnectAct, SIGNAL(triggered()), SLOT(disconnectDevice()) );

  SmartMenu *fileMenu = new SmartMenu(tr("&File"), 0, 0);
  fileMenu->addAction(connectAct);
  fileMenu->addAction(disconnectAct);

  _menus.insert(fileMenu);

  return _menus;
}

// IO members
void EventHandler::connectDevice(const QString &args) {
  // Connect a particular session's sockets.
  if (!args.isEmpty()) {
    // Update the host and port
    QStringList list = args.split(QRegExp("[:\\s]+"),
				  QString::SkipEmptyParts);
    qDebug() << list << list.size();
    if (list.size() == 2) {
      _socketReader->host(list.at(0));
      _socketReader->port(list.at(1).toInt());
    }
    
  }
  emit connectToHost();
  
}


void EventHandler::disconnectDevice() {
  // Disconnect a particular session's sockets.
  emit closeSocket();
}

void EventHandler::sendData(const QByteArray& ba) {
  // Send data to the sockets.
  emit sendToSocket(ba);
}


void EventHandler::socketReadData(const QByteArray& data) {
  QVariant* qv = new QVariant(data);
  QStringList tags("SocketReadData");
  postSession(qv, tags);
}

// Implementation-specific details: slots for successful operations
void EventHandler::displayMessage(const QString& message) {
  QVariant* qv = new QVariant(message);
  QStringList sl("DisplayData");
  postSession(qv, sl);
}


void EventHandler::socketOpened() {
  QVariant* qv = new QVariant();
  QStringList sl("SocketConnected");
  postSession(qv, sl);
  
  if (_pluginSession->isMUME()) sendMUMEGARequest();
}

void EventHandler::socketClosed() {
  QVariant* qv = new QVariant();
  QStringList sl("SocketDisconnected");
  postSession(qv, sl);
}


void EventHandler::sendMUMEGARequest() {
  qDebug() << "* sent MUME prompt GA request";
  sendData(QByteArray("~$#EP2\nG\n"));
}
