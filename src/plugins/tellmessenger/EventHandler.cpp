#include "EventHandler.h"
#include "SocketServer.h"

#include <QDebug>
#include <QEvent>
#include <QVariant>

#include "MClientEvent.h"
#include "AbstractPluginSession.h"
#include "ConfigEntry.h"

EventHandler::EventHandler(AbstractPluginSession *ps, MClientPlugin *mp)
    : MClientEventHandler(ps, mp) {
    
    _socketServer = new SocketServer(ps->session(), this);

    QString cfg = QString("config/");
  
    
    // Signals and slots
    connect(this, SIGNAL(closeSocket()), _socketServer, SLOT(closeSocket()));
    connect(this, SIGNAL(sendToSocket(const QByteArray &)),
	    _socketServer, SLOT(sendToSocket(const QByteArray &)));
    
}


EventHandler::~EventHandler() {
    delete _socketServer;
}


void EventHandler::customEvent(QEvent *e) {
  if (e->type() == 10001) {
    
      // Forward the event to the next in the chain
      forwardEvent(e);

      MClientEvent* me = static_cast<MClientEvent*>(e);
      
      if(me->dataTypes().contains("XMLTell")) {
	QByteArray ba = me->payload()->toByteArray();
	sendData(ba);
	
      }
    }
    
    else qDebug() << "TellMessenger got a customEvent of type" << e->type();
}



void EventHandler::disconnectDevice() {
    // Disconnect a particular session's sockets.
    emit closeSocket();
}


void EventHandler::sendData(const QByteArray& ba) {
    // Send data to the socket.
    emit sendToSocket(ba);
}
