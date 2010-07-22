#include "EventHandler.h"

#include <QEvent>
#include <QVariant>
#include <QDebug>
#include <QCoreApplication> // for sending command

#include "MClientEvent.h"

#include "PluginSession.h"
#include "CommandProcessor.h"

#include "MapperManager.h"
#include "mapwindow.h" // for grabbing the QWidget
#include "mmapper2pathmachine.h" // for menus
#include "mapcanvas.h" // for menus

// for menus
#include "SmartMenu.h"
#include <QAction>

EventHandler::EventHandler(AbstractPluginSession *ps, MClientPlugin *mp)
  : MClientDisplayHandler(ps, mp) {
  // Allowable Display Locations
  SET(_displayLocations, DL_FLOAT);
  _mapper = new MapperManager(this);
}


EventHandler::~EventHandler() {
  _mapper->disconnect();
  _mapper->deleteLater();
  qDebug() << "* removed MMapperPlugin for session"
	   << _pluginSession->session();
}


void EventHandler::customEvent(QEvent *e) {
  if (e->type() == 10001) {

    // Forward the event to the next in the chain
    forwardEvent(e);

    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    if (me->dataTypes().contains("XMLAll")) {

      //qDebug() << "display:" << me->dataTypes() << me->payload()->toString();
      
      bool display = true;
      if (me->dataTypes().contains("XMLExits")) {
	emit exits(me->payload()->toString());
	display = false;

      } else if (me->dataTypes().contains("XMLNone")) {
	emit mudOutput(me->payload()->toString());
	  
      } else if (me->dataTypes().contains("XMLName")) {
	emit name(me->payload()->toString());

      } else if (me->dataTypes().contains("XMLDescription")) {
	display = false;
	emit description(me->payload()->toString());

      } else if (me->dataTypes().contains("XMLDynamicDescription")) {
	emit dynamicDescription(me->payload()->toString());
	  
      } else if (me->dataTypes().contains("XMLPrompt")) {
	emit prompt(me->payload()->toString());
	  	  
      } else if (me->dataTypes().contains("XMLTerrain")) {
	emit terrain(me->payload()->toString());
       
      }

      if (display) {
	// These tags get forwarded to the CommandProcessor for actions/being displayed
	// TODO: add a boolean for this to be displayed, so actions still trigger
	MClientEvent* nme = new MClientEvent(*me);
	QCoreApplication::postEvent(_pluginSession->getCommand(), nme);
	//qDebug() << "* forwarding to CommandProcessor";
	
      }

    }
    else if (me->dataTypes().contains("XMLMove")) {
      emit move(me->payload()->toString());
    
    }
    else if(me->dataTypes().contains("MMapperInput")) {
      emit userInput(me->payload()->toString());
      
    }
    else if (me->dataTypes().contains("MMapperLoadMap")) {
      QString arguments = me->payload()->toString();
      if (arguments.isEmpty())
	displayMessage("#no file specified\n");
      else if (arguments == "!") // hack
	emit loadFile("/mnt/games/powwow/z263-264.mm2");
      else
	emit loadFile(arguments);
      
    }
    else if (me->dataTypes().contains("SocketConnected")) {
      emit onPlayMode();
      
    }
    else if (me->dataTypes().contains("SocketDisconnected")) {
      emit onOfflineMode();
      
    } 
  }
}


QWidget* EventHandler::createWidget(QWidget *parent) {
  _mapper->initDisplay(parent);
  return _mapper->getMapWindow();
}


void EventHandler::displayMessage(const QByteArray& message) {
  QVariant* qv = new QVariant(message);
  QStringList sl("DisplayData");
  postSession(qv, sl);
}


void EventHandler::log(const QString& s, const QString& message) {
  qDebug() << "* MMapperPlugin[" << s << "]: " << message;
}


void EventHandler::postCommand(const QByteArray &input) {
  qDebug() << "* MmapperPlugin submitting command" << input;
  QVariant *payload = new QVariant(input);
  QStringList tags("UserInput");
  MClientEventData *med = new MClientEventData(payload, tags,
					       _pluginSession->session());
  MClientEvent* me = new MClientEvent(med);
  QCoreApplication::postEvent(_pluginSession->getCommand(), me);

}


void EventHandler::foundDoors(const QStringList &list) {
  qDebug() << "* FOUND DOORS" << list;
  QVariant* qv = new QVariant(list);
  QStringList sl("AddTabHistory");
  postSession(qv, sl);
  
}


const MenuData& EventHandler::createMenus() {
  newAct = new QAction(QIcon(":/icons/new.png"), tr("&New"), 0);
  newAct->setShortcut(tr("Ctrl+N"));
  newAct->setStatusTip(tr("Create a new file"));
  connect(newAct, SIGNAL(triggered()), _mapper, SLOT(newFile()));

  openAct = new QAction(QIcon(":/icons/open.png"), tr("&Open..."), 0);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), _mapper, SLOT(open()));

  reloadAct = new QAction(QIcon(":/icons/reload.png"), tr("&Reload"), 0);
  reloadAct->setShortcut(tr("Ctrl+R"));
  reloadAct->setStatusTip(tr("Reload the current map"));
  connect(reloadAct, SIGNAL(triggered()), _mapper, SLOT(reload()));

  saveAct = new QAction(QIcon(":/icons/save.png"), tr("&Save"), 0);
  saveAct->setShortcut(tr("Ctrl+S"));
  saveAct->setStatusTip(tr("Save the document to disk"));
  connect(saveAct, SIGNAL(triggered()), _mapper, SLOT(save()));

  saveAsAct = new QAction(tr("Save &As..."), 0);
  saveAsAct->setStatusTip(tr("Save the document under a new name"));
  connect(saveAsAct, SIGNAL(triggered()), _mapper, SLOT(saveAs()));

  mergeAct = new QAction(QIcon(":/icons/merge.png"), tr("&Merge..."), 0);
  //mergeAct->setShortcut(tr("Ctrl+M"));
  mergeAct->setStatusTip(tr("Merge an existing file into current map"));
  connect(mergeAct, SIGNAL(triggered()), _mapper, SLOT(merge()));

  zoomInAct = new QAction(QIcon(":/icons/viewmag+.png"), tr("Zoom In"), 0);
  zoomInAct->setStatusTip(tr("Zooms In current map"));
  connect(zoomInAct, SIGNAL(triggered()), _mapper->getMapWindow()->getCanvas(), SLOT(zoomIn()));

  zoomOutAct = new QAction(QIcon(":/icons/viewmag-.png"), tr("Zoom Out"), 0);  zoomOutAct->setStatusTip(tr("Zooms Out current map"));
  connect(zoomOutAct, SIGNAL(triggered()), _mapper->getMapWindow()->getCanvas(), SLOT(zoomOut()));

  layerUpAct = new QAction(QIcon(":/icons/layerup.png"), tr("Layer Up"), 0);
  layerUpAct->setStatusTip(tr("Layer Up"));
  connect(layerUpAct, SIGNAL(triggered()), _mapper, SLOT(onLayerUp()));
  layerDownAct = new QAction(QIcon(":/icons/layerdown.png"), tr("Layer Down"), 0);
  layerDownAct->setStatusTip(tr("Layer Down"));
  connect(layerDownAct, SIGNAL(triggered()), _mapper, SLOT(onLayerDown()));

  modeConnectionSelectAct = new QAction(QIcon(":/icons/connectionselection.png"), tr("Select Connection"), 0);
  modeConnectionSelectAct->setStatusTip(tr("Select Connection"));
  modeConnectionSelectAct->setCheckable(true);
  connect(modeConnectionSelectAct, SIGNAL(triggered()), _mapper, SLOT(onModeConnectionSelect()));
  modeRoomSelectAct = new QAction(QIcon(":/icons/roomselection.png"), tr("Select Rooms"), 0);
  modeRoomSelectAct->setStatusTip(tr("Select Rooms"));
  modeRoomSelectAct->setCheckable(true);
  connect(modeRoomSelectAct, SIGNAL(triggered()), _mapper, SLOT(onModeRoomSelect()));
  modeMoveSelectAct = new QAction(QIcon(":/icons/mapmove.png"), tr("Move map"), 0);
  modeMoveSelectAct->setStatusTip(tr("Move Map"));
  modeMoveSelectAct->setCheckable(true);
  connect(modeMoveSelectAct, SIGNAL(triggered()), _mapper, SLOT(onModeMoveSelect()));
  modeInfoMarkEditAct = new QAction(QIcon(":/icons/infomarksedit.png"), tr("Edit Info Marks"), 0);
  modeInfoMarkEditAct->setStatusTip(tr("Edit Info Marks"));
  modeInfoMarkEditAct->setCheckable(true);
  connect(modeInfoMarkEditAct, SIGNAL(triggered()), _mapper, SLOT(onModeInfoMarkEdit()));

  createRoomAct = new QAction(QIcon(":/icons/roomcreate.png"), tr("Create New Rooms"), 0);
  createRoomAct->setStatusTip(tr("Create New Rooms"));
  createRoomAct->setCheckable(true);
  connect(createRoomAct, SIGNAL(triggered()), _mapper, SLOT(onModeCreateRoomSelect()));

  createConnectionAct = new QAction(QIcon(":/icons/connectioncreate.png"), tr("Create New Connection"), 0);
  createConnectionAct->setStatusTip(tr("Create New Connection"));
  createConnectionAct->setCheckable(true);
  connect(createConnectionAct, SIGNAL(triggered()), _mapper, SLOT(onModeCreateConnectionSelect()));

  createOnewayConnectionAct = new QAction(QIcon(":/icons/onewayconnectioncreate.png"), tr("Create New Oneway Connection"), 0);
  createOnewayConnectionAct->setStatusTip(tr("Create New Oneway Connection"));
  createOnewayConnectionAct->setCheckable(true);
  connect(createOnewayConnectionAct, SIGNAL(triggered()), _mapper, SLOT(onModeCreateOnewayConnectionSelect()));

  modeActGroup = new QActionGroup(0);
  modeActGroup->setExclusive(true);
  modeActGroup->addAction(modeMoveSelectAct);
  modeActGroup->addAction(modeRoomSelectAct);
  modeActGroup->addAction(modeConnectionSelectAct);
  modeActGroup->addAction(createRoomAct);
  modeActGroup->addAction(createConnectionAct);
  modeActGroup->addAction(createOnewayConnectionAct);
  modeActGroup->addAction(modeInfoMarkEditAct);
  modeMoveSelectAct->setChecked(true);

  editRoomSelectionAct = new QAction(QIcon(":/icons/roomedit.png"), tr("Edit Selected Rooms"), 0);
  editRoomSelectionAct->setStatusTip(tr("Edit Selected Rooms"));
  connect(editRoomSelectionAct, SIGNAL(triggered()), _mapper, SLOT(onEditRoomSelection()));

  deleteRoomSelectionAct = new QAction(QIcon(":/icons/roomdelete.png"), tr("Delete Selected Rooms"), 0);
  deleteRoomSelectionAct->setStatusTip(tr("Delete Selected Rooms"));
  connect(deleteRoomSelectionAct, SIGNAL(triggered()), _mapper, SLOT(onDeleteRoomSelection()));

  moveUpRoomSelectionAct = new QAction(QIcon(":/icons/roommoveup.png"), tr("Move Up Selected Rooms"), 0);
  moveUpRoomSelectionAct->setStatusTip(tr("Move Up Selected Rooms"));
  connect(moveUpRoomSelectionAct, SIGNAL(triggered()), _mapper, SLOT(onMoveUpRoomSelection()));
  moveDownRoomSelectionAct = new QAction(QIcon(":/icons/roommovedown.png"), tr("Move Down Selected Rooms"), 0);
  moveDownRoomSelectionAct->setStatusTip(tr("Move Down Selected Rooms"));
  connect(moveDownRoomSelectionAct, SIGNAL(triggered()), _mapper, SLOT(onMoveDownRoomSelection()));
  mergeUpRoomSelectionAct = new QAction(QIcon(":/icons/roommergeup.png"), tr("Merge Up Selected Rooms"), 0);
  mergeUpRoomSelectionAct->setStatusTip(tr("Merge Up Selected Rooms"));
  connect(mergeUpRoomSelectionAct, SIGNAL(triggered()), _mapper, SLOT(onMergeUpRoomSelection()));
  mergeDownRoomSelectionAct = new QAction(QIcon(":/icons/roommergedown.png"), tr("Merge Down Selected Rooms"), 0);
  mergeDownRoomSelectionAct->setStatusTip(tr("Merge Down Selected Rooms"));
  connect(mergeDownRoomSelectionAct, SIGNAL(triggered()), _mapper, SLOT(onMergeDownRoomSelection()));
  connectToNeighboursRoomSelectionAct = new QAction(QIcon(":/icons/roomconnecttoneighbours.png"), tr("Connect room(s) to its neighbour rooms"), 0);
  connectToNeighboursRoomSelectionAct->setStatusTip(tr("Connect room(s) to its neighbour rooms"));
  connect(connectToNeighboursRoomSelectionAct, SIGNAL(triggered()), _mapper, SLOT(onConnectToNeighboursRoomSelection()));

  findRoomsAct = new QAction(QIcon(":/icons/roomfind.png"), tr("&Find Rooms"), 0);
  findRoomsAct->setStatusTip(tr("Find Matching Rooms"));
  findRoomsAct->setShortcut(tr("Ctrl+F"));
  connect(findRoomsAct, SIGNAL(triggered()), _mapper, SLOT(onFindRoom()));

  releaseAllPathsAct = new QAction(QIcon(":/icons/cancel.png"), tr("Release All Paths"), 0);
  releaseAllPathsAct->setStatusTip(tr("Release All Paths"));
  releaseAllPathsAct->setCheckable(false);
  connect(releaseAllPathsAct, SIGNAL(triggered()), _mapper->getPathMachine(), SLOT(releaseAllPaths()));

  forceRoomAct = new QAction(QIcon(":/icons/force.png"), tr("Force Path Machine to selected Room"), 0);
  forceRoomAct->setStatusTip(tr("Force Path Machine to selected Room"));
  forceRoomAct->setCheckable(false);
  forceRoomAct->setEnabled(FALSE);
  connect(forceRoomAct, SIGNAL(triggered()), _mapper->getMapWindow()->getCanvas(), SLOT(forceMapperToRoom()));

  roomActGroup = new QActionGroup(0);
  roomActGroup->setExclusive(false);
  roomActGroup->addAction(editRoomSelectionAct);
  roomActGroup->addAction(deleteRoomSelectionAct);
  roomActGroup->addAction(moveUpRoomSelectionAct);
  roomActGroup->addAction(moveDownRoomSelectionAct);
  roomActGroup->addAction(mergeUpRoomSelectionAct);
  roomActGroup->addAction(mergeDownRoomSelectionAct);
  roomActGroup->addAction(connectToNeighboursRoomSelectionAct);
  roomActGroup->setEnabled(FALSE);

  deleteConnectionSelectionAct = new QAction(QIcon(":/icons/connectiondelete.png"), tr("Delete Selected Connection"), 0);
  deleteConnectionSelectionAct->setStatusTip(tr("Delete Selected Connection"));
  connect(deleteConnectionSelectionAct, SIGNAL(triggered()), _mapper, SLOT(onDeleteConnectionSelection()));

  connectionActGroup = new QActionGroup(0);
  connectionActGroup->setExclusive(false);
  //connectionActGroup->addAction(editConnectionSelectionAct);
  connectionActGroup->addAction(deleteConnectionSelectionAct);
  connectionActGroup->setEnabled(FALSE);

  playModeAct = new QAction(QIcon(":/icons/online.png"), tr("Switch to play mode"), 0);
  playModeAct->setStatusTip(tr("Switch to play mode - no new rooms are created"));
  playModeAct->setCheckable(true);
  connect(playModeAct, SIGNAL(triggered()), _mapper, SLOT(onPlayMode()));

  mapModeAct = new QAction(QIcon(":/icons/map.png"), tr("Switch to mapping mode"), 0);
  mapModeAct->setStatusTip(tr("Switch to mapping mode - new rooms are created when moving"));
  mapModeAct->setCheckable(true);
  connect(mapModeAct, SIGNAL(triggered()), _mapper, SLOT(onMapMode()));

  offlineModeAct = new QAction(QIcon(":/icons/play.png"), tr("Switch to offline emulation mode"), 0);
  offlineModeAct->setStatusTip(tr("Switch to offline emulation mode - you can learn areas offline"));
  offlineModeAct->setCheckable(true);
  connect(offlineModeAct, SIGNAL(triggered()), _mapper, SLOT(onOfflineMode()));

  preferencesAct = new QAction(QIcon(":/icons/preferences.png"), tr("MMapper configuration"), this);
  preferencesAct->setShortcut(tr("Ctrl+P"));
  preferencesAct->setStatusTip(tr("MMapper2 configuration"));
  connect(preferencesAct, SIGNAL(triggered()), _mapper, SLOT(onPreferences()));

  mapModeActGroup = new QActionGroup(0);
  mapModeActGroup->setExclusive(true);
  mapModeActGroup->addAction(playModeAct);
  mapModeActGroup->addAction(mapModeAct);
  mapModeActGroup->addAction(offlineModeAct);
  mapModeActGroup->setEnabled(true);
  offlineModeAct->setChecked(true);

  SmartMenu *mapperMenu = new SmartMenu(tr("&Mapper"), 5, 2);
  SmartMenu *viewMenu = new SmartMenu(tr("&View"), 10, 2);
  SmartMenu *fileMenu = new SmartMenu(tr("&File"), 2, 0);
  QMenu *roomMenu = mapperMenu->addMenu(tr("&Rooms"));
  QMenu *connectionMenu = mapperMenu->addMenu(tr("&Connections"));

  fileMenu->addSeparator();
  fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);
  fileMenu->addAction(reloadAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);
  fileMenu->addAction(mergeAct);
  fileMenu->addSeparator();

  mapperMenu->addSeparator();
  mapperMenu->addAction(playModeAct);
  mapperMenu->addAction(mapModeAct);
  mapperMenu->addAction(offlineModeAct);
  mapperMenu->addSeparator();
  mapperMenu->addAction(modeRoomSelectAct);
  mapperMenu->addAction(modeConnectionSelectAct);
  mapperMenu->addAction(modeMoveSelectAct);
  mapperMenu->addAction(modeInfoMarkEditAct);
  mapperMenu->addSeparator();
  mapperMenu->addAction(preferencesAct);

  roomMenu->addAction(findRoomsAct);
  roomMenu->addSeparator();
  roomMenu->addAction(createRoomAct);
  roomMenu->addAction(editRoomSelectionAct);
  roomMenu->addAction(deleteRoomSelectionAct);
  roomMenu->addAction(moveUpRoomSelectionAct);
  roomMenu->addAction(moveDownRoomSelectionAct);
  roomMenu->addAction(mergeUpRoomSelectionAct);
  roomMenu->addAction(mergeDownRoomSelectionAct);
  roomMenu->addAction(connectToNeighboursRoomSelectionAct);

  connectionMenu->addAction(createConnectionAct);
  connectionMenu->addAction(createOnewayConnectionAct);
  //connectionMenu->addAction(editConnectionSelectionAct);
  connectionMenu->addAction(deleteConnectionSelectionAct);

  viewMenu->addSeparator();
  viewMenu->addAction(zoomInAct);
  viewMenu->addAction(zoomOutAct);
  viewMenu->addSeparator();
  viewMenu->addAction(layerUpAct);
  viewMenu->addAction(layerDownAct);
  viewMenu->addSeparator();
  viewMenu->addAction(releaseAllPathsAct);
  viewMenu->addAction(forceRoomAct);
  viewMenu->addSeparator();

  _menus.insert(fileMenu);
  _menus.insert(mapperMenu);
  _menus.insert(viewMenu);

  return _menus;
}
