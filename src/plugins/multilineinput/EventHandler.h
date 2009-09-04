#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientDisplayHandler.h"

class QEvent;
class InputWidget;

class EventHandler : public MClientDisplayHandler {
    Q_OBJECT
    
    public:
        EventHandler(QWidget* parent=0);
        ~EventHandler();

        void customEvent(QEvent* e);

	QWidget* createWidget();

public slots:
        void sendUserInput(const QString&, bool);
        void displayMessage(const QString &); 

 signals:
	void setEchoMode(bool);
	void showCommandHistory();

 private:
	InputWidget *_widget;
};


#endif /* EVENTHANDLER_H */
