#ifndef MCLIENTEVENT_H
#define MCLIENTEVENT_H

#include <QEvent>

#include "MClientEventData.h"

#include <QSharedDataPointer>
#include <QStringList>

//class MClientEventData;

class QVariant;
class QSharedData;
class QString;

class MClientEvent : public QEvent {
    
    public:
        MClientEvent(MClientEventData* payload, const QStringList type, const QString session);
        MClientEvent(MClientEventData* payload, const QStringList type);
	MClientEvent(const MClientEvent &me);

        ~MClientEvent();
        QStringList dataTypes() const;
        QVariant* payload();
        QSharedDataPointer<MClientEventData> shared() const;

        const QString& session() const;
        void session(const QString);

    protected:
        QStringList _dataTypes;

        // Here's where we store our data
        QSharedDataPointer<MClientEventData> _payload;

        QString _session;
                
};


#endif /* MCLIENTEVENT_H */
