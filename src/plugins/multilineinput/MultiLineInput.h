#ifndef SIMPLELINEINPUT_H
#define SIMPLELINEINPUT_H

#include "MClientPlugin.h"

#include <QHash>
#include <QPointer>

class EventHandler;
class QEvent;

class MultiLineInput : public MClientPlugin {
    Q_OBJECT
    
    public:
        MultiLineInput(QObject* parent=0);
        ~MultiLineInput();

        // Plugin members
        void configure();
        bool startSession(PluginSession *ps);
        bool stopSession(PluginSession *ps);

	MClientEventHandler* getEventHandler(QString s);

    private:
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
};


#endif /* SIMPLELINEINPUT_H */
