#include "EventHandler.h"

#include <QDebug>
#include <QEvent>
#include <QVariant>

#include "SmartMenu.h"
#include <QAction>

#include "MClientEvent.h"
#include "PluginSession.h"

EventHandler::EventHandler(QObject* parent) : MClientEventHandler(parent) {
  _openSocket = false;
}


EventHandler::~EventHandler() {
}


void EventHandler::customEvent(QEvent *e) {
  if (e->type() == 10000)
    engineEvent(e);
  else if (e->type() == 10001) {
    
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    if(me->dataTypes().contains("SendToSocketData")) {
      QByteArray ba = me->payload()->toByteArray();
      sendData(ba);
      
    } else if (me->dataTypes().contains("ConnectToHost")) {
      //QString arg = me->payload()->toString();
      connectDevice();
      
    } else if (me->dataTypes().contains("DisconnectFromHost")) {
      //QString arg = me->payload()->toString();
      disconnectDevice();
      
    } else if (me->dataTypes().contains("MUMEPromptGARequest")) {
      //do not allow server to suppress go-aheads (MUME is backwards)
      sendData(QByteArray("~$#EP2\nG\n"));
      
    }

    
  }
  else 
    qDebug() << "SocketManagerIO got a customEvent of type" << e->type();
}


// Menu
const MenuData EventHandler::createMenus() {
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
void EventHandler::connectDevice() {
    if (_openSocket) {
      displayMessage("#connection is already open. "
		     "Use '#zap' to disconnect it.\n");

    } else {
      // Connect a particular session's sockets.
      emit connectToHost();

    }
}


void EventHandler::disconnectDevice() {
    if (!_openSocket) {
      displayMessage("#no open connections to zap.\n");

    } else {
      // Disconnect a particular session's sockets.
      emit closeSocket();
    }
}


void EventHandler::sendData(const QByteArray& ba) {
  // Send data to the sockets.
  if (!_openSocket)
    qDebug() << "! Socket is not open!";
  else
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
  _openSocket = true;
  QVariant* qv = new QVariant();
  QStringList sl("SocketConnected");
  postSession(qv, sl);
}

void EventHandler::socketClosed() {
  _openSocket = false;
  QVariant* qv = new QVariant();
  QStringList sl("SocketDisconnected");
  postSession(qv, sl);
}
