#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientDisplayHandler.h"

class QEvent;
class ClientTextEdit;
class PluginSession;

class EventHandler : public MClientDisplayHandler {
    Q_OBJECT
    
    public:
         EventHandler(PluginSession *ps, MClientPlugin *mp);
        ~EventHandler();

        void customEvent(QEvent* e);

	QWidget* createWidget();

	const MenuData& createMenus();

 signals:
	void displayText(const QString&);
	void userInput(const QString&);

 private:
	ClientTextEdit *_widget;
};


#endif /* EVENTHANDLER_H */
