#ifndef REMOTEEDIT_H
#define REMOTEEDIT_H

#include "MClientPlugin.h"

class PluginSession;
class EventHandler;

class RemoteEdit : public MClientPlugin {
    Q_OBJECT
    
    public:
        RemoteEdit(QObject* parent=0);
        ~RemoteEdit();

        void configure();
        bool startSession(PluginSession *ps);
        bool stopSession(PluginSession *ps);

	MClientEventHandler* getEventHandler(QString s);

 private:
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
};


#endif /* REMOTEEDIT_H */
