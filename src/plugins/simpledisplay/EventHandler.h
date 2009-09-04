#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientDisplayHandler.h"

class QEvent;
class ClientTextEdit;

class EventHandler : public MClientDisplayHandler {
    Q_OBJECT
    
    public:
        EventHandler(QWidget* parent=0);
        ~EventHandler();

        void customEvent(QEvent* e);

	QWidget* createWidget();

 signals:
	void displayText(const QString&);
	void userInput(const QString&);

 private:
	ClientTextEdit *_widget;
};


#endif /* EVENTHANDLER_H */
