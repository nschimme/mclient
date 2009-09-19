#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientDisplayHandler.h"

class QEvent;
class DisplayWidget;
class DisplayParser;

class EventHandler : public MClientDisplayHandler {
    Q_OBJECT
    
    public:
         EventHandler(PluginSession *ps, MClientPlugin *mp);
        ~EventHandler();

        void customEvent(QEvent* e);

	QWidget* createWidget();

 private:
	DisplayWidget *_widget;
	DisplayParser *_parser;

 signals:
	void displayData(const QString &);
	void userInput(const QString &);
};


#endif /* EVENTHANDLER_H */
