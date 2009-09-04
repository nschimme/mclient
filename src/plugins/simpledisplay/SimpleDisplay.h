#ifndef SIMPLEDISPLAY_H
#define SIMPLEDISPLAY_H

#include "MClientPlugin.h"

#include <QPointer>

class EventHandler;
class QEvent;

class SimpleDisplay : public MClientPlugin {
    Q_OBJECT
    
    public:
        SimpleDisplay(QObject* parent=0);
        ~SimpleDisplay();

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


#endif /* SIMPLEDISPLAY_H */
