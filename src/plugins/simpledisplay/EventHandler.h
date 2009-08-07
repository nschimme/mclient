#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientEventHandler.h"

class QEvent;

class EventHandler : public MClientEventHandler {
    Q_OBJECT
    
    public:
        EventHandler(QObject* parent=0);
        ~EventHandler();

        void customEvent(QEvent* e);

 signals:
	void displayText(const QString&);
	void userInput(const QString&);
};


#endif /* EVENTHANDLER_H */
