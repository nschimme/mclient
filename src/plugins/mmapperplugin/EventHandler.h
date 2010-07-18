#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientDisplayHandler.h"

#include <QActionGroup>
#include <QAction>
#include <QMenu>
#include <QPointer>

class QEvent;
class MapperManager;
class AbstractPluginSession;

class EventHandler : public MClientDisplayHandler {
    Q_OBJECT
    
    public:
         EventHandler(AbstractPluginSession *ps, MClientPlugin *mp);
        ~EventHandler();

        void customEvent(QEvent* e);

	QWidget* createWidget(QWidget *parent=0);

	const MenuData& createMenus();

public slots:
	void displayMessage(const QByteArray &);
        void log(const QString&, const QString&);
	void postCommand(const QByteArray &);
	void foundDoors(const QStringList &);

 signals:
	void name(QString);
	void description(const QString &);
	void dynamicDescription(const QString &);
	void prompt(QString);
	void exits(QString);
	void move(const QString &);
	void terrain(const QString &);

	void userInput(QString);
	void mudOutput(const QString&);

	void loadFile(const QString&);

	void onPlayMode();
	void onOfflineMode();

 protected:
	// Mapper actions
	QPointer<QAction> newAct;
	QPointer<QAction> openAct;
	QPointer<QAction> mergeAct;
	QPointer<QAction> reloadAct;
	QPointer<QAction> saveAct;
	QPointer<QAction> saveAsAct;
	QPointer<QAction> zoomInAct;
	QPointer<QAction> zoomOutAct;

	QPointer<QAction> layerUpAct;
	QPointer<QAction> layerDownAct;

	QPointer<QAction> modeConnectionSelectAct;
	QPointer<QAction> modeRoomSelectAct;
	QPointer<QAction> modeMoveSelectAct;
	QPointer<QAction> modeInfoMarkEditAct;

	QPointer<QAction> createRoomAct;
	QPointer<QAction> createConnectionAct;
	QPointer<QAction> createOnewayConnectionAct;

	QPointer<QAction> playModeAct;
	QPointer<QAction> mapModeAct;
	QPointer<QAction> offlineModeAct;

	QPointer<QActionGroup> mapModeActGroup;
	QPointer<QActionGroup> modeActGroup;
	QPointer<QActionGroup> roomActGroup;
	QPointer<QActionGroup> connectionActGroup;

	QPointer<QAction> editRoomSelectionAct;
	QPointer<QAction> deleteRoomSelectionAct;
	QPointer<QAction> deleteConnectionSelectionAct;

	QPointer<QAction> moveUpRoomSelectionAct;
	QPointer<QAction> moveDownRoomSelectionAct;
	QPointer<QAction> mergeUpRoomSelectionAct;
	QPointer<QAction> mergeDownRoomSelectionAct;
	QPointer<QAction> connectToNeighboursRoomSelectionAct;

	QPointer<QAction> findRoomsAct;
	QPointer<QAction> preferencesAct;

	QPointer<QAction> forceRoomAct;
	QPointer<QAction> releaseAllPathsAct;
	
 private:
	MapperManager *_mapper;

	friend class MapperManager;
};


#endif /* EVENTHANDLER_H */
