#ifndef MCLIENTEVENT_H
#define MCLIENTEVENT_H

#include "MClientEventData.h"

#include <QEvent>
#include <QSharedDataPointer>
#include <QStringList>
#include <QVariant>

class QVariant;
class QSharedData;
class QString;

class MClientEvent : public QEvent {
    
    public:
        MClientEvent(MClientEventData *payload);
	MClientEvent(const MClientEvent &other);
        ~MClientEvent();

        MClientEventData* shared();

        QStringList dataTypes();
        QVariant* payload();
        const QString& session();

    protected:
	// Payload is contained here
        QSharedDataPointer<MClientEventData> _p;

};

#endif /* MCLIENTEVENT_H */
