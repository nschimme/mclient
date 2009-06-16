#ifndef MCLIENTEVENTDATA_H
#define MCLIENTEVENTDATA_H

#include <QSharedData>

#include <QString>
#include <QStringList>
#include <QVariant>

class MClientEventData : public QSharedData {
    
    public:
        MClientEventData(QVariant *payload, const QStringList types,
			 const QString session);
	MClientEventData(const MClientEventData &other);
        ~MClientEventData();

        QVariant* payload() { return &_payload; };
	const QString& session() { return _session; }
	QStringList dataTypes() { return _dataTypes; }
        
    private:
        QVariant _payload;
	QString _session;
	QStringList _dataTypes;

};


#endif /* MCLIENTEVENTDATA_H */
