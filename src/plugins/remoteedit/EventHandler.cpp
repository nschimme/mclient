#include "EventHandler.h"

#include <QEvent>
#include <QVariant>
#include <QStringList>

#include <QDebug>

#include "MClientEvent.h"
#include "EditSessionProcess.h"

EventHandler::EventHandler(PluginSession *ps, MClientPlugin *mp)
  : MClientEventHandler(ps, mp) {
  _state = NORMAL;
}


EventHandler::~EventHandler() {
}


void EventHandler::customEvent(QEvent *e) {
  if (e->type() == 10000)
    engineEvent(e);

  else if (e->type() == 10001) {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    if (me->dataTypes().contains("XMLEdit") ||
	me->dataTypes().contains("XMLView")) {
      if (_state != NORMAL)
	qWarning() << "! Remote editing protocol somehow received a key";

      _key = me->payload()->toInt();
      _state = RECEIVED_KEY;
      
    }
    else if (me->dataTypes().contains("XMLEditTitle")) {
      if (_state == RECEIVED_KEY) {
	_title = me->payload()->toByteArray();
	_state = RECEIVED_TITLE;
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
      _key = 0;
      _title = me->payload()->toByteArray();
      _state = RECEIVED_TITLE;
      /*
      if (_state == RECEIVED_KEY) {
	_title = me->payload()->toByteArray();
	_state = RECEIVED_TITLE;
      }
      else {
	qWarning() << "! Remote editing protocol never got a key"
		   << "; discarding title";
	_state = NORMAL;

      }
      */
      
    }
    else if (me->dataTypes().contains("XMLViewBody")) {
      if (_state == RECEIVED_TITLE) {
	_body = me->payload()->toByteArray();
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
    
  }
  else {
    qDebug() << "! RemoteEdit somehow received the wrong kind of event...";
    
  }
}

void EventHandler::editSession(int key, const QByteArray &title,
			       const QByteArray &body) {
  EditSessionProcess *process = new EditSessionProcess(key, title, body, this);

  connect(process, SIGNAL(sendToSocket(int, const QByteArray &)),
	  this, SLOT(sendToSocket(int, const QByteArray &)));
}


void EventHandler::viewSession(int key, const QByteArray &title,
			       const QByteArray &body) {
  new ViewSessionProcess(key, title, body, this);
}


void EventHandler::sendToSocket(int key, const QByteArray &data) {
  qDebug() << "* Edit session" << key << "writing to socket" << data;
  QVariant* qv = new QVariant(data);
  QStringList sl("SocketWriteData");
  postSession(qv, sl);

}
