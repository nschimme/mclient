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

public slots:
        void sendUserInput(const QString&, bool);
        void displayMessage(const QString &); 

 signals:
	void setEchoMode(bool);
	void showCommandHistory();
};


#endif /* EVENTHANDLER_H */
