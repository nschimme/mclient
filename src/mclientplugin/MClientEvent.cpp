#include "MClientEvent.h"

#include "MClientEventData.h"

#include <QDebug>
#include <QVariant>
#include <QString>

MClientEvent::MClientEvent(MClientEventData* payload, const QStringList types,
			   const QString session) 
        : QEvent(QEvent::Type(10001)) {

    _payload = QSharedDataPointer<MClientEventData>(payload);
    _dataTypes = types;
    _session = session;
}


MClientEvent::MClientEvent(MClientEventData* payload, const QStringList types) 
        : QEvent(QEvent::Type(10001)) {
//    qDebug() << "MClientEvent::MClientEvent()";

    // We take ownership of payload!
    _payload = QSharedDataPointer<MClientEventData>(payload);
    //_payload->refs();
    _dataTypes = types;
}

MClientEvent::MClientEvent(const MClientEvent& me) 
        : QEvent(QEvent::Type(10001)), _payload(me._payload),
        _dataTypes(me._dataTypes), _session(me.session()) {
     //       qDebug() << "copying shit, yo";
/*
//    MClientEvent& me2 = const_cast<MClientEvent&>(me);
    _dataType = me.dataType();
    _payload = QSharedDataPointer<MClientEventData>(me.shared());
//    me2.relinquishPayload();
*/
}


MClientEvent::~MClientEvent() {
  //qDebug() << "MClientEvent::~MClientEvent()";
}


QStringList MClientEvent::dataTypes() const {
    return _dataTypes;
}


const QString& MClientEvent::session() const {
    return _session;
}


void MClientEvent::session(const QString s) {
    _session = s;
}


QVariant* MClientEvent::payload() {
    return _payload->payload();
}


QSharedDataPointer<MClientEventData> MClientEvent::shared() const {
    return _payload;
}

/*
void MClientEvent::relinquishPayload() {
    qDebug() << "giving up payload... :(";
    _payload = 0;
}
*/
