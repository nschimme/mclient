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
        bool loadSettings();
        bool saveSettings() const;
        bool startSession(QString s);
        bool stopSession(QString s);

	MClientEventHandler* getEventHandler(QString s);

    private:
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
};


#endif /* SIMPLELINEINPUT_H */
