#include "MClientEventData.h"

#include <QDebug>

MClientEventData::MClientEventData(QVariant* payload, const QStringList types,
				   const QString session) : QSharedData() {
    //qDebug() << "* creating a shared data containing" << payload;
    //qDebug() << "* payload is:" << payload->toByteArray().data();
    _payload = *payload;
    _dataTypes = types;
    _session = session;
    
}

MClientEventData::MClientEventData(const MClientEventData &other)
  : QSharedData() {
  _payload = other._payload;
  _dataTypes = other._dataTypes;
  _session = other._session;
  
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
  //  qDebug() << "* deleting payload" << _payload << _dataTypes << _session << ref;
  // Removed this because it would delete the plugin after transfer of
  // a MClientEngineEvent

  //delete _payload;
}
