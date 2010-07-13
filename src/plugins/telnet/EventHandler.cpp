#include "EventHandler.h"

#include "MClientEvent.h"
#include "TelnetParser.h"

#include <QEvent>
#include <QVariant>
#include <QDebug>
#include <QStringList>

EventHandler::EventHandler(AbstractPluginSession *ps, MClientPlugin *mp)
  : MClientEventHandler(ps, mp) {
  _telnetParser = new TelnetParser(this);
}


EventHandler::~EventHandler() {
  _telnetParser->disconnect();
  _telnetParser->deleteLater();
}


void EventHandler::customEvent(QEvent *e) {
  if (e->type() == 10001) {

    // Forward the event to the next in the chain
    forwardEvent(e);

    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    const QStringList &types = me->dataTypes();
    foreach(const QString &s, types) {
      if (s.startsWith("SocketReadData")) {
	emit readData(me->payload()->toByteArray());
	
      }
      else if(s.startsWith("SocketWriteData")) {	
	emit sendData(me->payload()->toByteArray());

      }
      else if(s.startsWith("SocketConnected")) {
	emit socketConnected();

      }
      else if(s.startsWith("SocketDisconnected")) {
	emit socketDisconnected();

      }
      else if(s.startsWith("DimensionsChanged")) {
	const QList<QVariant> &par = me->payload()->toList();
	emit windowSizeChanged (par.first().toInt(), par.last().toInt());

      }
    }
  }
  else {
    qDebug() << "Telnet somehow received the wrong kind of event...";
    
  }
}

void EventHandler::socketWrite(const QByteArray &data) {
  QVariant* qv = new QVariant(data);
  QStringList sl("SendToSocketData");  
  postSession(qv, sl);
}

void EventHandler::echoModeChanged(bool echoMode) {
  QVariant* qv = new QVariant(echoMode);
  QStringList sl("EchoMode");
  postSession(qv, sl);
}

void EventHandler::displayData(const QString &unicodeData, bool goAhead) {
  QVariant *qv = new QVariant(unicodeData);
  QStringList sl("TelnetData");
  postSession(qv, sl);

  if (goAhead) {
    qv = new QVariant();
    QStringList sl("TelnetGA");
    postSession(qv, sl);
  }
}
