#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientEventHandler.h"

class QEvent;
class ScriptEngine;

class EventHandler : public MClientEventHandler {
    Q_OBJECT
    
    public:
        EventHandler(QObject* parent=0);
        ~EventHandler();

        void customEvent(QEvent* e);

public slots:
	void displayMessage(const QString&);
        void log(const QString&, const QString&);

 signals:
	void name(QString);
	void description(const QString &);
	void dynamicDescription(const QString &);
	void prompt(QString);
	void exits(QString);
	void move(QString);
	void terrain(QString);

	void userInput(QString);
	void mudOutput(const QString&);

	void loadFile(const QString&);

	void onPlayMode();
	void onOfflineMode();
};


#endif /* EVENTHANDLER_H */
