#ifndef REMOTEEDIT_H
#define REMOTEEDIT_H

#include "MClientPlugin.h"

class AbstractPluginSession;
class EventHandler;

class RemoteEdit : public MClientPlugin {
    Q_OBJECT
    
    public:
        RemoteEdit(QObject* parent=0);
        ~RemoteEdit();

        void configure();
        bool startSession(AbstractPluginSession *ps);
        bool stopSession(const QString &session);

	MClientEventHandler* getEventHandler(const QString &s);

 private:
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
};


#endif /* REMOTEEDIT_H */
