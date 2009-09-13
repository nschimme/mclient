#ifndef MUMEXML_H
#define MUMEXML_H

#include "MClientPlugin.h"

class PluginSession;
class EventHandler;

class MumeXML : public MClientPlugin {
    Q_OBJECT
    
    public:
        MumeXML(QObject* parent=0);
        ~MumeXML();

        // From MClientFilterPlugin
        void configure();
        bool startSession(PluginSession *ps);
        bool stopSession(PluginSession *ps);

	MClientEventHandler* getEventHandler(QString s);

 private:
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
};


#endif /* MUMEXML_H */
