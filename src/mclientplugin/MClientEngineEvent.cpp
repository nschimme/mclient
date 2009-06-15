#include "MClientEngineEvent.h"

#include "MClientEventData.h"

#include <QDebug>
#include <QVariant>
#include <QString>

MClientEngineEvent::MClientEngineEvent(MClientEventData* payload,
				       const MClientEngineEventDataType type,
				       const QString &session) 
        : QEvent(QEvent::Type(10000)) {

    _payload = QSharedDataPointer<MClientEventData>(payload);
    _dataType = type;
    _session = session;
}


MClientEngineEvent::MClientEngineEvent(MClientEventData* payload,
				       const MClientEngineEventDataType type) 
        : QEvent(QEvent::Type(10000)) {

    // We take ownership of payload!
    _payload = QSharedDataPointer<MClientEventData>(payload);
    _dataType = type;
}


MClientEngineEvent::MClientEngineEvent(const MClientEngineEvent& me)
  : QEvent(QEvent::Type(10000)) {
  // Makes a copy of the event
  _payload = me._payload;
  _dataType = me._dataType;
  _session = me.session();
}


MClientEngineEvent::~MClientEngineEvent() {
}


MClientEngineEventDataType MClientEngineEvent::dataType() const {
    return _dataType;
}


const QString& MClientEngineEvent::session() const {
    return _session;
}


void MClientEngineEvent::session(const QString s) {
    _session = s;
}


QVariant* MClientEngineEvent::payload() {
    return _payload->payload();
}


QSharedDataPointer<MClientEventData> MClientEngineEvent::shared() const {
    return _payload;
}
