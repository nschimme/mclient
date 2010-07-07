#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientDisplayHandler.h"

class QEvent;
class StackedWidget;
class AbstractPluginSession;

class EventHandler : public MClientDisplayHandler {
    Q_OBJECT
    
    public:
         EventHandler(AbstractPluginSession *ps, MClientPlugin *mp);
        ~EventHandler();

        void customEvent(QEvent* e);

	QWidget* createWidget(QWidget *parent=0);

public slots:
        void sendUserInput(const QString&, bool);
        void displayMessage(const QString &); 

 signals:
	void setEchoMode(bool);
	void showCommandHistory();
	void addTabHistory(const QStringList &);

 private:
	StackedWidget *_widget;
};


#endif /* EVENTHANDLER_H */
