#ifndef SIMPLEDISPLAY_H
#define SIMPLEDISPLAY_H

#include "MClientDisplayPlugin.h"

#include <QPointer>

class ClientTextEdit;
class EventHandler;
class QEvent;

class SimpleDisplay : public MClientDisplayPlugin {
    Q_OBJECT
    
    public:
        SimpleDisplay(QWidget* parent=0);
        ~SimpleDisplay();

        // Plugin members
        void configure();
        bool loadSettings();
        bool saveSettings() const;
        bool startSession(QString s);
        bool stopSession(QString s);

	MClientEventHandler* getEventHandler(QString s);

        // Display members
        bool initDisplay(QString s);
        QWidget* getWidget(QString s);

  protected:
	void run();

    private:
	QHash<QString, QPointer<ClientTextEdit> > _widgets;
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
};


#endif /* SIMPLEDISPLAY_H */
