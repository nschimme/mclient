#include "EventHandler.h"

#include <QDebug>
#include <QEvent>
#include <QVariant>

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
    }
  }
  else 
    qDebug() << "SocketManagerIO got a customEvent of type" << e->type();
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
  if (!_openSocket) {
    displayMessage("#no open connections. Use '#connect' to open a "
		   "connection.\n");
    
  }
  else {
    emit sendToSocket(ba);
  }
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
