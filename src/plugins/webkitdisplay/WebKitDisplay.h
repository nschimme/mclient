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
        bool startSession(AbstractPluginSession *ps);
        bool stopSession(const QString &session);

	MClientEventHandler* getEventHandler(const QString &session);

    private:
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
};


#endif /* WEBKITDISPLAY_H */
