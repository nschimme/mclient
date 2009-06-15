#ifndef MCLIENTENGINEEVENT_H
#define MCLIENTENGINEEVENT_H

#include <QEvent>

#include "MClientEventData.h"

#include <QSharedDataPointer>
#include <QStringList>

class QVariant;
class QSharedData;
class QString;

enum MClientEngineEventDataType
  { EE_MANAGER_POST = 0, EE_DATATYPE_UPDATE
  };

class MClientEngineEvent : public QEvent {
    
    public:
        MClientEngineEvent(MClientEventData* payload,
			   const MClientEngineEventDataType type,
			   const QString &session);
        MClientEngineEvent(MClientEventData* payload,
			   const MClientEngineEventDataType type);
        MClientEngineEvent (const MClientEngineEvent &me);
        ~MClientEngineEvent();

	// Setters
	void session(const QString);
	
	// Getters
        MClientEngineEventDataType dataType() const;
        QVariant* payload();
        QSharedDataPointer<MClientEventData> shared() const;
	const QString& session() const;

    protected:
	MClientEngineEventDataType _dataType;
        QSharedDataPointer<MClientEventData> _payload;
        QString _session;
	
};


#endif /* MCLIENTENGINEEVENT_H */
