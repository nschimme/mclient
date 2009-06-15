#include "MClientEventData.h"

#include <QDebug>
#include <QVariant>


MClientEventData::MClientEventData(QVariant* payload) : QSharedData() {
    //qDebug() << "* creating a shared data containing" << payload;
    //qDebug() << "* payload is:" << payload->toByteArray().data();
    _payload = payload;
    
}

MClientEventData::MClientEventData(const MClientEventData& med)
  : QSharedData() {
  _payload = new QVariant(*med._payload);
      /*
    if (med._payload) {
      qDebug() << "* copied payload from" << med._payload << "to" 
	       << _payload;
      qDebug() << "* old payload is:" << med._payload->toByteArray().data();
      qDebug() << "* new payload is:" << _payload->toByteArray().data();
    }
      */
}


MClientEventData::~MClientEventData() {
  //qDebug() << "* deleting payload" << _payload << ref;
  //qDebug() << "Deleting payload";

  // Removed this because it would delete the plugin after transfer of
  // a MClientEngineEvent

  //delete _payload;
}


QVariant* MClientEventData::payload() {
//    qDebug() << ref << this << "returns payload";
    return _payload;
}

void MClientEventData::refs() const {
    qDebug() << "refs" << ref;
}
