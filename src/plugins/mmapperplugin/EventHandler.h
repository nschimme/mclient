#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientDisplayHandler.h"

class QEvent;
class MapperManager;

class EventHandler : public MClientDisplayHandler {
    Q_OBJECT
    
    public:
        EventHandler(QWidget* parent=0);
        ~EventHandler();

        void customEvent(QEvent* e);

	QWidget* createWidget();

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
	
 private:
	MapperManager *_mapper;
};


#endif /* EVENTHANDLER_H */
