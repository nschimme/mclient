#ifndef MCLIENTEVENTHANDLER_H
#define MCLIENTEVENTHANDLER_H

#include <QObject>
#include <QMultiHash>
#include <QStringList>

class PluginSession;
class MClientEventData;

class MClientEventHandler : public QObject {
    Q_OBJECT
    
    public:
        MClientEventHandler(QObject* parent=0);
        ~MClientEventHandler();

        // Handles custom events
        virtual void customEvent(QEvent *e)=0;

	// Handles MClientEngineEvents
	void engineEvent(QEvent *e);

	// Receive the PluginSession reference upon load
	void setPluginSession(PluginSession *ps);

public slots:
        // Post an event to the PluginSession
	void postSession(QVariant* payload, QStringList types);

        // Post an event to the PluginManager
	void postManager(QVariant* payload, QStringList types, 
			 QString session);


    protected:
	QMultiHash<QString, QObject*> _receivesTypes;

	// References
	PluginSession *_pluginSession;

};


#endif /* MCLIENTEVENTHANDLER_H */
