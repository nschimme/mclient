#include "MClientEvent.h"

#include "MClientEventData.h"

#include <QDebug>

MClientEvent::MClientEvent(MClientEventData *payload)
        : QEvent(QEvent::Type(10001)) {
  _p = QSharedDataPointer<MClientEventData>(payload);
}


MClientEvent::MClientEvent(const MClientEvent &other) 
  : QEvent(other), _p(other._p) {
}


MClientEvent::~MClientEvent() {
  //qDebug() << "MClientEvent::~MClientEvent()" << *_p->payload();
}


QStringList MClientEvent::dataTypes() {
  return _p->dataTypes();
}

QVariant* MClientEvent::payload() {
  return _p->payload(); 
}

const QString& MClientEvent::session() {
  return _p->session();
}

MClientEventData* MClientEvent::shared() {
  return _p.data();
}
