#ifndef MUMEXML_H
#define MUMEXML_H

#include "MClientFilterPlugin.h"

class EventHandler;

class MumeXML : public MClientFilterPlugin {
    Q_OBJECT
    
    public:
        MumeXML(QObject* parent=0);
        ~MumeXML();

        // From MClientFilterPlugin
        void configure();
        bool loadSettings();
        bool saveSettings() const;
        bool startSession(QString s);
        bool stopSession(QString s);

	MClientEventHandler* getEventHandler(QString s);

 private:
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
};


#endif /* MUMEXML_H */
