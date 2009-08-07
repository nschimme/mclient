#ifndef SIMPLELINEINPUT_H
#define SIMPLELINEINPUT_H

#include "MClientDisplayPlugin.h"

#include <QHash>
#include <QPointer>

class InputWidget;
class EventHandler;
class QEvent;

class MultiLineInput : public MClientDisplayPlugin {
    Q_OBJECT
    
    public:
        MultiLineInput(QWidget* parent=0);
        ~MultiLineInput();

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

    private:
	QHash<QString, QPointer<InputWidget> > _widgets;
	QHash<QString, QPointer<EventHandler> > _eventHandlers;
};


#endif /* SIMPLELINEINPUT_H */
