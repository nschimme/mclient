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
        bool startSession(AbstractPluginSession *ps);
        bool stopSession(const QString &session);

	MClientEventHandler* getEventHandler(const QString &session);

    private:
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
};


#endif /* SIMPLELINEINPUT_H */
