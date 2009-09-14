#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientDisplayHandler.h"

class QEvent;
class MapperManager;
class PluginSession;

class EventHandler : public MClientDisplayHandler {
    Q_OBJECT
    
    public:
         EventHandler(PluginSession *ps, MClientPlugin *mp);
        ~EventHandler();

        void customEvent(QEvent* e);

	QWidget* createWidget();

	const MenuData& createMenus();

public slots:
	void displayMessage(const QByteArray &);
        void log(const QString&, const QString&);
	void postCommand(const QByteArray &);

 signals:
	void name(QString);
	void description(const QString &);
	void dynamicDescription(const QString &);
	void prompt(QString);
	void exits(QString);
	void move(const QString &);
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
