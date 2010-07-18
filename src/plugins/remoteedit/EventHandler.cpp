#include "EventHandler.h"

#include <QEvent>
#include <QVariant>
#include <QStringList>

#include <QDebug>

#include "AbstractPluginSession.h"
#include "MClientEvent.h"
#include "EditSessionProcess.h"

EventHandler::EventHandler(AbstractPluginSession *ps, MClientPlugin *mp)
  : MClientEventHandler(ps, mp) {
  _state = NORMAL;
}


EventHandler::~EventHandler() {
}


void EventHandler::customEvent(QEvent *e) {
  if (e->type() == 10001) {

    // Forward the event to the next in the chain
    forwardEvent(e);

    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    if (me->dataTypes().contains("XMLEdit")) {
      if (_state != NORMAL)
	qWarning() << "! Remote editing protocol somehow received a key";

      _key = me->payload()->toInt();
      _state = RECEIVED_KEY;
      qDebug() << "* Remote editing protocol got edit key" << _key;
      
    }
    else if (me->dataTypes().contains("XMLEditTitle")) {
      if (_state == RECEIVED_KEY) {
	_title = me->payload()->toByteArray();
	_state = RECEIVED_TITLE;
	qDebug() << "* Remote editing protocol got edit title" << _title;
      }
      else {
	qWarning() << "! Remote editing protocol never got a key"
		   << "; discarding title";
	_state = NORMAL;

      }
      
    }
    else if (me->dataTypes().contains("XMLEditBody")) {
      if (_state == RECEIVED_TITLE) {
	_body = me->payload()->toByteArray();
	qDebug() << "* Remote editing protocol got edit body" << _body;
	editSession(_key, _title, _body);

      }
      else if (_state == RECEIVED_KEY)
	qWarning() << "! Remote editing protocol never got a title"
		   << "; discarding body";
      else
	qWarning() << "! Remote editing protocol never got a key"
		   << "; discarding body";
      
      _state = NORMAL;
      
    }
    else if (me->dataTypes().contains("XMLViewTitle")) {
      if (_state == NORMAL) {
	_key = -1;
	_title = me->payload()->toByteArray();
	_state = RECEIVED_TITLE;
	qDebug() << "* Remote editing protocol got view title" << _title;
      }
      
    }
    else if (me->dataTypes().contains("XMLViewBody")) {
      if (_state == RECEIVED_TITLE) {
	_body = me->payload()->toByteArray();
	qDebug() << "* Remote editing protocol got view body" << _body;
	viewSession(_key, _title, _body);

      }
      else if (_state == RECEIVED_KEY)
	qWarning() << "! Remote editing protocol never got a title"
		   << "; discarding body";
      else
	qWarning() << "! Remote editing protocol never got a key"
		   << "; discarding body";
      
      _state = NORMAL;
      
    }
    else if (me->dataTypes().contains("MUMEIdentifyRequest")) {
      sendMUMEIdentifyRequest();

    }
    else if (me->dataTypes().contains("SocketConnected")) {
      if (_pluginSession->isMUME()) sendMUMEIdentifyRequest();

    }
  }
  else {
    qDebug() << "! RemoteEdit somehow received the wrong kind of event...";
    
  }
}

void EventHandler::editSession(int key, const QByteArray &title,
			       const QByteArray &body) {
  EditSessionProcess *process = new EditSessionProcess(key, title, body, this);

  connect(process, SIGNAL(sendToSocket(const QByteArray &)),
	  SLOT(sendToSocket(const QByteArray &)));
}


void EventHandler::viewSession(int key, const QByteArray &title,
			       const QByteArray &body) {
  new ViewSessionProcess(key, title, body, this);
}


void EventHandler::sendMUMEIdentifyRequest() {
  qDebug() << "* sent MUME identify request";
  sendToSocket(QByteArray("~$#EI\n"));
}


void EventHandler::sendToSocket(const QByteArray &data) {
  QVariant* qv = new QVariant(data);
  QStringList sl("SocketWriteData");
  postSession(qv, sl);

}
