#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientDisplayHandler.h"

class QEvent;
class MapperManager;
class PluginSession;
class QAction;
class QActionGroup;

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

	// Mapper actions
    QAction *newAct;
    QAction *openAct;
    QAction *mergeAct;
    QAction *reloadAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *prevWindowAct;
    QAction *nextWindowAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;

    QAction *layerUpAct;
    QAction *layerDownAct;

    QAction *modeConnectionSelectAct;
    QAction *modeRoomSelectAct;
    QAction *modeMoveSelectAct;
    QAction *modeInfoMarkEditAct;

    QAction *createRoomAct;
    QAction *createConnectionAct;
    QAction *createOnewayConnectionAct;

    QAction *playModeAct;
    QAction *mapModeAct;
    QAction *offlineModeAct;

    QActionGroup *mapModeActGroup;
    QActionGroup *modeActGroup;
    QActionGroup *roomActGroup;
    QActionGroup *connectionActGroup;
    QActionGroup *groupManagerGroup;

    QAction *editRoomSelectionAct;
    QAction *editConnectionSelectionAct;
    QAction *deleteRoomSelectionAct;
    QAction *deleteConnectionSelectionAct;

    QAction *moveUpRoomSelectionAct;
    QAction *moveDownRoomSelectionAct;
    QAction *mergeUpRoomSelectionAct;
    QAction *mergeDownRoomSelectionAct;
    QAction *connectToNeighboursRoomSelectionAct;

    QAction *findRoomsAct;

    QAction *forceRoomAct;
    QAction *releaseAllPathsAct;

};


#endif /* EVENTHANDLER_H */
