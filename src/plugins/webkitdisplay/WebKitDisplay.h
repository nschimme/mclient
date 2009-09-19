#ifndef WEBKITDISPLAY_H
#define WEBKITDISPLAY_H

#include "MClientPlugin.h"

#include <QPointer>

class QEvent;
class EventHandler;

class WebKitDisplay : public MClientPlugin {
    Q_OBJECT
    
    public:
        WebKitDisplay(QWidget* parent=0);
        ~WebKitDisplay();

        // Plugin members
        void configure();
        bool startSession(PluginSession *ps);
        bool stopSession(PluginSession *ps);

	MClientEventHandler* getEventHandler(QString s);

    private:
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
};


#endif /* WEBKITDISPLAY_H */
