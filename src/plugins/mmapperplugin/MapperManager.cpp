/*
 * MMapperPlugin is based upon MMapper version 2.0.4 beta
 *
 * This is NOT the latest release of MMapper2 which has a new file
 * compression map system. You will need an older map.
 *
 */

#include <QMessageBox>
#include <QFileDialog>

#include "MapperManager.h"
#include "MainWindow.h"

#include "MMapperPluginParser.h"
#include "EventHandler.h"

#include "configuration.h" // TODO: remove this

#include "mapwindow.h"
#include "mapcanvas.h"
#include "mapdata.h"
#include "mapstorage.h"

#include "mmapper2pathmachine.h"
#include "customaction.h"
#include "prespammedpath.h"

#include "roomselection.h"
#include "connectionselection.h"
#include "roompropertysetter.h"

#include "findroomsdlg.h"
#include "roomeditattrdlg.h"
#include "progresscounter.h"
#include "mapstorage/filesaver.h"

MapperManager::MapperManager(EventHandler *eh, QObject *parent)
  : QThread(parent), _eventHandler(eh) {
  qRegisterMetaType<CommandQueue>("CommandQueue");
  qRegisterMetaType<DirectionType>("DirectionType");
  qRegisterMetaType<DoorActionType>("DoorActionType");
  qRegisterMetaType<Coordinate>("Coordinate");

  _roomSelection = NULL;
  _connectionSelection = NULL;

  connect(_eventHandler, SIGNAL(onPlayMode()), SLOT(onPlayMode()));
  connect(_eventHandler, SIGNAL(onOfflineMode()), SLOT(onOfflineMode()));

  _mapData = new MapData();
  _mapData->setObjectName("MapData");
  _mapData->start();
  qDebug("MapData loaded");

  _prespammedPath = new PrespammedPath(parent);
  qDebug("PrespammedPath loaded");

  _mapWindow = new MapWindow;
  getMapWindow()->getCanvas()->init(_mapData, _prespammedPath);
  qDebug("MapWindow/DockWidget loaded");

  _pathMachine = new Mmapper2PathMachine();
  _pathMachine->setObjectName("Mmapper2PathMachine");
  _pathMachine->start();
  qDebug("PathMachine loaded");

  _propertySetter = new RoomPropertySetter();
  _propertySetter->setObjectName("RoomPropertySetter");
  _propertySetter->start();
  qDebug("RoomPropertySetter loaded");

  _findRoomsDlg = new FindRoomsDlg(_mapData);
  _findRoomsDlg->setObjectName("FindRoomsDlg");
  qDebug("FindRooms loaded");

  connect(this, SIGNAL(log( const QString&, const QString& )), _eventHandler, SLOT(log( const QString&, const QString& )));
  connect(_pathMachine, SIGNAL(log( const QString&, const QString& )), _eventHandler, SLOT(log( const QString&, const QString& )));
  connect(_mapData, SIGNAL(log( const QString&, const QString& )), _eventHandler, SLOT(log( const QString&, const QString& )));
  connect(_mapWindow->getCanvas(), SIGNAL(log( const QString&, const QString& )), _eventHandler, SLOT(log( const QString&, const QString& )));
  connect(_findRoomsDlg, SIGNAL(log( const QString&, const QString& )), _eventHandler, SLOT(log( const QString&, const QString& )));

  connect(_mapWindow->getCanvas(), SIGNAL(newRoomSelection(const RoomSelection*)), SLOT(newRoomSelection(const RoomSelection*)));
  connect(_mapWindow->getCanvas(), SIGNAL(newConnectionSelection(ConnectionSelection*)), SLOT(newConnectionSelection(ConnectionSelection*)));

  connect(_pathMachine, SIGNAL(lookingForRooms(RoomRecipient*, const Coordinate & )), _mapData, SLOT(lookingForRooms(RoomRecipient*, const Coordinate & )));
  connect(_pathMachine, SIGNAL(lookingForRooms(RoomRecipient*, ParseEvent* )), _mapData, SLOT(lookingForRooms(RoomRecipient*, ParseEvent* )));
  connect(_pathMachine, SIGNAL(lookingForRooms(RoomRecipient*, uint )), _mapData, SLOT(lookingForRooms(RoomRecipient*, uint )));
  connect(_pathMachine, SIGNAL(playerMoved(const Coordinate & )), _mapWindow->getCanvas(), SLOT(moveMarker(const Coordinate &)));
  connect(_mapData, SIGNAL(clearingMap()), _pathMachine, SLOT(releaseAllPaths()));

  connect(_mapWindow->getCanvas(), SIGNAL(setCurrentRoom(uint)), _pathMachine, SLOT(setCurrentRoom(uint)));
  connect(_mapWindow->getCanvas(), SIGNAL(charMovedEvent(ParseEvent*)), _pathMachine, SLOT(event(ParseEvent*)));

  connect(_mapData, SIGNAL(mapSizeChanged(const Coordinate &, const Coordinate &)), _mapWindow, SLOT(setScrollBars(const Coordinate &, const Coordinate &)));
  connect(_mapWindow->getCanvas(), SIGNAL(roomPositionChanged()), _pathMachine, SLOT(retry()));
  connect(_prespammedPath, SIGNAL(update()), _mapWindow->getCanvas(), SLOT(update()));
  connect(_mapData, SIGNAL(onDataLoaded()), _mapWindow->getCanvas(), SLOT(dataLoaded()) );

  connect(_findRoomsDlg, SIGNAL(center(qint32, qint32)), _mapWindow, SLOT(center(qint32, qint32)));

  // Create Parser
  _parser = new MMapperPluginParser(this);

  connect(getParser(), SIGNAL(event(ParseEvent* )),
          getPathMachine(), SLOT(event(ParseEvent* )));
  connect(getParser(), SIGNAL(releaseAllPaths()),
          getPathMachine(), SLOT(releaseAllPaths()));
  connect(getParser(), SIGNAL(showPath(CommandQueue, bool)),
          getPrespammedPath(), SLOT(setPath(CommandQueue, bool)));

  connect(_eventHandler, SIGNAL(name(const QString&)),
	  getParser(), SLOT(name(const QString&)));
  connect(_eventHandler, SIGNAL(description(const QString&)),
	  getParser(), SLOT(description(const QString&)));
  connect(_eventHandler, SIGNAL(dynamicDescription(const QString&)),
	  getParser(), SLOT(dynamicDescription(const QString&)));
  connect(_eventHandler, SIGNAL(exits(const QString&)),
	  getParser(), SLOT(exits(const QString&)));
  connect(_eventHandler, SIGNAL(prompt(const QString&)),
	  getParser(), SLOT(prompt(const QString&)));
  connect(_eventHandler, SIGNAL(move(const QString &)),
	  getParser(), SLOT(move(const QString &)));
  connect(_eventHandler, SIGNAL(terrain(const QString &)),
	  getParser(), SLOT(terrain(const QString &)));
  
  connect(_eventHandler, SIGNAL(userInput(QString)),
	  getParser(), SLOT(userInput(QString)),
	  Qt::DirectConnection);
  connect(_eventHandler, SIGNAL(mudOutput(const QString&)),
	  getParser(), SLOT(mudOutput(const QString&)),
	  Qt::DirectConnection);

  connect(getParser(), SIGNAL(sendToUser(const QByteArray &)),
 	  _eventHandler, SLOT(displayMessage(const QByteArray &)),
	  Qt::DirectConnection);
  connect(getParser(), SIGNAL(sendToMud(const QByteArray &)),
 	  _eventHandler, SLOT(postCommand(const QByteArray &)),
	  Qt::DirectConnection);
  
  connect(_eventHandler, SIGNAL(loadFile(const QString&)),
	  this, SLOT(loadFile(const QString&)));
  
  // Start in offline mode
  onOfflineMode();

  /** Threaded attempt
  qDebug() << "Creating MapWindow...";
  _mapWindow = new MapWindow;
  qDebug() << "MapWindow loaded" << _mapWindow->thread();

  _findRoomsDlg = 0;
  _roomSelection = NULL;
  _connectionSelection = NULL;
  */

  // auto load the map
  if (Config().m_autoLoadWorld && Config().m_autoLoadFileName!="")
    loadFile(Config().m_autoLoadFileName);

  qDebug() << "* MapperManager thread:" << QThread::currentThread();
}

MapperManager::~MapperManager() {
  exit();
  wait();
}


void MapperManager::run() {
  /** Threaded attempt
  _mapData = new MapData();
  _mapData->setObjectName("MapData");
  _mapData->start();
  qDebug("MapData loaded");

  _prespammedPath = new PrespammedPath();
  qDebug("PrespammedPath loaded");

  //_mapWindow = new MapWindow(_mapData, _prespammedPath);
  getMapWindow()->getCanvas()->init(_mapData, _prespammedPath);
  qDebug("MapWindow initialized");

  _pathMachine = new Mmapper2PathMachine();
  _pathMachine->setObjectName("Mmapper2PathMachine");
  _pathMachine->start();
  qDebug("PathMachine loaded");

  _propertySetter = new RoomPropertySetter();
  _propertySetter->setObjectName("RoomPropertySetter");
  _propertySetter->start();
  qDebug("RoomPropertySetter loaded");

  connect(this, SIGNAL(log( const QString&, const QString& )), _eventHandler, SLOT(log( const QString&, const QString& )));
  connect(_pathMachine, SIGNAL(log( const QString&, const QString& )), _eventHandler, SLOT(log( const QString&, const QString& )));
  connect(_mapData, SIGNAL(log( const QString&, const QString& )), _eventHandler, SLOT(log( const QString&, const QString& )));
  connect(_mapWindow->getCanvas(), SIGNAL(log( const QString&, const QString& )), _eventHandler, SLOT(log( const QString&, const QString& )));

  connect(_mapWindow->getCanvas(), SIGNAL(newRoomSelection(const RoomSelection*)), SLOT(newRoomSelection(const RoomSelection*)));
  connect(_mapWindow->getCanvas(), SIGNAL(newConnectionSelection(ConnectionSelection*)), SLOT(newConnectionSelection(ConnectionSelection*)));

  connect(_pathMachine, SIGNAL(lookingForRooms(RoomRecipient*, const Coordinate & )), _mapData, SLOT(lookingForRooms(RoomRecipient*, const Coordinate & )));
  connect(_pathMachine, SIGNAL(lookingForRooms(RoomRecipient*, ParseEvent* )), _mapData, SLOT(lookingForRooms(RoomRecipient*, ParseEvent* )));
  connect(_pathMachine, SIGNAL(lookingForRooms(RoomRecipient*, uint )), _mapData, SLOT(lookingForRooms(RoomRecipient*, uint )));
  connect(_pathMachine, SIGNAL(playerMoved(const Coordinate & )), _mapWindow->getCanvas(), SLOT(moveMarker(const Coordinate &)));
  connect(_mapData, SIGNAL(clearingMap()), _pathMachine, SLOT(releaseAllPaths()));

  connect(_mapWindow->getCanvas(), SIGNAL(setCurrentRoom(uint)), _pathMachine, SLOT(setCurrentRoom(uint)));
  connect(_mapWindow->getCanvas(), SIGNAL(charMovedEvent(ParseEvent*)), _pathMachine, SLOT(event(ParseEvent*)));

  connect(_mapData, SIGNAL(mapSizeChanged(const Coordinate &, const Coordinate &)), _mapWindow, SLOT(setScrollBars(const Coordinate &, const Coordinate &)));
  connect(_mapWindow->getCanvas(), SIGNAL(roomPositionChanged()), _pathMachine, SLOT(retry()));
  connect(_prespammedPath, SIGNAL(update()), _mapWindow->getCanvas(), SLOT(update()));
  connect(_mapData, SIGNAL(onDataLoaded()), _mapWindow->getCanvas(), SLOT(dataLoaded()) );

  // Create Parser
  _parser = new MMapperPluginParser(this);

  connect(getParser(), SIGNAL(event(ParseEvent* )),
          getPathMachine(), SLOT(event(ParseEvent* )));
  connect(getParser(), SIGNAL(releaseAllPaths()),
          getPathMachine(), SLOT(releaseAllPaths()));
  connect(getParser(), SIGNAL(showPath(CommandQueue, bool)),
          getPrespammedPath(), SLOT(setPath(CommandQueue, bool)));

  connect(_eventHandler, SIGNAL(name(const QString&)),
	  getParser(), SLOT(name(const QString&)));
  connect(_eventHandler, SIGNAL(description(const QString&)),
	  getParser(), SLOT(description(const QString&)));
  connect(_eventHandler, SIGNAL(dynamicDescription(const QString&)),
	  getParser(), SLOT(dynamicDescription(const QString&)));
  connect(_eventHandler, SIGNAL(exits(const QString&)),
	  getParser(), SLOT(exits(const QString&)));
  connect(_eventHandler, SIGNAL(prompt(const QString&)),
	  getParser(), SLOT(prompt(const QString&)));
  connect(_eventHandler, SIGNAL(move(const QString&)),
	  getParser(), SLOT(move(const QString&)));
  
  connect(_eventHandler, SIGNAL(userInput(QString)),
	  getParser(), SLOT(userInput(QString)),
	  Qt::DirectConnection);
  connect(_eventHandler, SIGNAL(mudOutput(const QString&)),
	  getParser(), SLOT(mudOutput(const QString&)),
	  Qt::DirectConnection);

  connect(getParser(), SIGNAL(sendToUser(const QByteArray &)),
 	  _eventHandler, SLOT(displayMessage(const QByteArray &)),
	  Qt::DirectConnection);
  
  connect(_eventHandler, SIGNAL(loadFile(const QString&)),
	  this, SLOT(loadFile(const QString&)));
  
  // Start in offline mode
  onOfflineMode();

  exec();
  */
}


void MapperManager::newFile()
{
  getMapWindow()->getCanvas()->clearRoomSelection();
  getMapWindow()->getCanvas()->clearConnectionSelection();

  if (maybeSave())
  {
    AbstractMapStorage *storage = (AbstractMapStorage*) new MapStorage(*_mapData , "");
    connect(storage, SIGNAL(onNewData()), getMapWindow()->getCanvas(), SLOT(dataLoaded()));
    connect(storage, SIGNAL(log(const QString&, const QString&)), _eventHandler, SLOT(log(const QString&, const QString&)));
    storage->newData();
    delete(storage);
  }
}

void MapperManager::merge()
{
  QString fileName = QFileDialog::getOpenFileName(0,"Choose map file ...","","MMapper2 (*.mm2);;MMapper (*.map)");
  if (!fileName.isEmpty())
  {
    QFile *file = new QFile(fileName);

    if (!file->open(QFile::ReadOnly))
    {
      QMessageBox::warning(0, tr("Application"),
                           tr("Cannot read file %1:\n%2.")
                               .arg(fileName)
                               .arg(file->errorString()));

      getMapWindow()->getCanvas()->setEnabled(true);
      delete file;
      return;
    }

    //MERGE
    progressDlg = new QProgressDialog(getMapWindow());
    QPushButton *cb = new QPushButton("Abort ...");
    cb->setEnabled(false);
    progressDlg->setCancelButton ( cb );
    progressDlg->setLabelText("Importing map...");
    progressDlg->setCancelButtonText("Abort");
    progressDlg->setMinimum(0);
    progressDlg->setMaximum(100);
    progressDlg->setValue(0);
    progressDlg->show();

    getMapWindow()->getCanvas()->clearRoomSelection();
    getMapWindow()->getCanvas()->clearConnectionSelection();

    AbstractMapStorage *storage = new MapStorage(*_mapData , fileName, file);
    connect(storage, SIGNAL(onDataLoaded()), getMapWindow()->getCanvas(), SLOT(dataLoaded()));
    connect(storage->progressCounter(), SIGNAL(onPercentageChanged(quint32)), this, SLOT(percentageChanged(quint32)));
    connect(storage, SIGNAL(log(const QString&, const QString&)), this, SLOT(log(const QString&, const QString&)));

//     ActionManager *actMgr = ActionManager::self();
//     actMgr->disableActions(true);
    getMapWindow()->getCanvas()->hide();
    if (storage->canLoad()) storage->mergeData();
    getMapWindow()->getCanvas()->show();
//     actMgr->disableActions(false);
//     actMgr->cutAct->setEnabled(false);
//     actMgr->copyAct->setEnabled(false);
//     actMgr->pasteAct->setEnabled(false);

    delete(storage);
    delete progressDlg;

    if (_mainWindow)
      _mainWindow->statusBar()->showMessage(tr("File merged"), 2000);
    delete file;
  }
}

void MapperManager::open()
{
  if (maybeSave())
  {
    QString fileName = QFileDialog::getOpenFileName(_mapWindow,"Choose map file ...","","MMapper2 (*.mm2);;MMapper (*.map)");
    if (!fileName.isEmpty())
      loadFile(fileName);
  }
}

void MapperManager::reload()
{
  if (maybeSave())
  {
    loadFile(_mapData->getFileName());
  }
}

bool MapperManager::save()
{
  if (_mapData->getFileName().isEmpty())
  {
    return saveAs();
  }
  else
  {
    return saveFile(_mapData->getFileName());
  }
}

bool MapperManager::saveAs()
{
  QFileDialog save(_mapWindow, "Choose map file name ...");
  save.setFileMode(QFileDialog::AnyFile);
  save.setDirectory(QDir::current());
  save.setFilter("MMapper2 (*.mm2)");
  save.setDefaultSuffix("mm2");
  save.setAcceptMode(QFileDialog::AcceptSave);
  QStringList fileNames;
  if (save.exec()) {
    fileNames = save.selectedFiles();
  }

  if (fileNames.isEmpty()) {
    if (_mainWindow)
      _mainWindow->statusBar()->showMessage(tr("No filename provided"), 2000);
    return false;
  }

  return saveFile(fileNames[0]);
}


bool MapperManager::maybeSave()
{
  if ( _mapData->dataChanged() )
  {
    int ret = QMessageBox::warning(_mapWindow, tr("Application"),
                                   tr("The document has been modified.\n"
                                       "Do you want to save your changes?"),
                                       QMessageBox::Yes | QMessageBox::Default,
                                       QMessageBox::No,
                                       QMessageBox::Cancel | QMessageBox::Escape);
    if (ret == QMessageBox::Yes)
      return save();
    else if (ret == QMessageBox::Cancel)
      return false;
  }
  return true;
}

void MapperManager::loadFile(const QString &fileName)
{
  //getMapWindow()->getCanvas()->setEnabled(false);

  QFile *file = new QFile(fileName);
  //QIODevice *file = KFilterDev::deviceForFile( filename, "application/x-gzip", TRUE );

  if (!file->open(QFile::ReadOnly))
  {
    QMessageBox::warning(_mapWindow, tr("Application"),
                         tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file->errorString()));

    getMapWindow()->getCanvas()->setEnabled(true);
    delete file;
    return;
  }

  //LOAD
  progressDlg = new QProgressDialog(getMapWindow());
  QPushButton *cb = new QPushButton("Abort ...");
  cb->setEnabled(false);
  progressDlg->setCancelButton ( cb );
  progressDlg->setLabelText("Loading map...");
  progressDlg->setCancelButtonText("Abort");
  progressDlg->setMinimum(0);
  progressDlg->setMaximum(100);
  progressDlg->setValue(0);
  progressDlg->show();

  getMapWindow()->getCanvas()->clearRoomSelection();
  getMapWindow()->getCanvas()->clearConnectionSelection();

  AbstractMapStorage *storage = (AbstractMapStorage*) new MapStorage(*_mapData , fileName, file);
  connect(storage, SIGNAL(onDataLoaded()), getMapWindow()->getCanvas(), SLOT(dataLoaded()));
  connect(storage->progressCounter(), SIGNAL(onPercentageChanged(quint32)), this, SLOT(percentageChanged(quint32)));
  connect(storage, SIGNAL(log(const QString&, const QString&)), this, SLOT(log(const QString&, const QString&)));

//   ActionManager *actMgr = ActionManager::self();
//   actMgr->disableActions(true);
  getMapWindow()->getCanvas()->hide();
  if (storage->canLoad()) storage->loadData();
  getMapWindow()->getCanvas()->show();
//   actMgr->disableActions(false);
//   actMgr->cutAct->setEnabled(false);
//   actMgr->copyAct->setEnabled(false);
//   actMgr->pasteAct->setEnabled(false);

  delete(storage);
  delete progressDlg;

  //setCurrentFile(fileName);
  if (_mainWindow)  
      _mainWindow->statusBar()->showMessage(tr("File loaded"), 2000);
  delete file;
  //getMapWindow()->getCanvas()->setEnabled(true);
}

void MapperManager::percentageChanged(quint32 p)
{
  if(!progressDlg) return;
  progressDlg->setValue(p);

  qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
  //qApp->processEvents();
}

bool MapperManager::saveFile(const QString &fileName, bool baseMapOnly )
{
  getMapWindow()->getCanvas()->setEnabled(false);

  FileSaver saver;
  try
  {
    saver.open( fileName );
  }
  catch ( std::exception &e )
  {
    QMessageBox::warning(NULL, tr("Application"),
                         tr("Cannot write file %1:\n%2.")
                         .arg(fileName)
                         .arg(e.what()));
    getMapWindow()->getCanvas()->setEnabled(true);
    return false;
  }


  //SAVE
  progressDlg = new QProgressDialog(getMapWindow());
  QPushButton *cb = new QPushButton("Abort ...");
  cb->setEnabled(false);
  progressDlg->setCancelButton ( cb );
  progressDlg->setLabelText("Saving map...");
  progressDlg->setCancelButtonText("Abort");
  progressDlg->setMinimum(0);
  progressDlg->setMaximum(100);
  progressDlg->setValue(0);
  progressDlg->show();

  std::auto_ptr<AbstractMapStorage> storage( new MapStorage(*_mapData , fileName, &saver.file()) );
  connect(storage->progressCounter(), SIGNAL(onPercentageChanged(quint32)), this, SLOT(percentageChanged(quint32)));
  connect(storage.get(), SIGNAL(log(const QString&, const QString&)), this, SLOT(log(const QString&, const QString&)));

//   ActionManager *actMgr = ActionManager::self();
//   actMgr->disableActions(true);
  //getMapWindow()->getCanvas()->hide();
  if (storage->canSave()) storage->saveData( baseMapOnly );
  //getMapWindow()->getCanvas()->show();
//   actMgr->disableActions(false);
//   actMgr->cutAct->setEnabled(false);
//   actMgr->copyAct->setEnabled(false);
//   actMgr->pasteAct->setEnabled(false);

  delete progressDlg;

  try
  {
    saver.close();
  }
  catch ( std::exception &e )
  {
    QMessageBox::warning(NULL, tr("Application"),
                         tr("Cannot write file %1:\n%2.")
                         .arg(fileName)
                         .arg(e.what()));
    getMapWindow()->getCanvas()->setEnabled(true);
    return false;
  }
  
  /*
  if ( !baseMapOnly )
    setCurrentFile(fileName);
  statusBar()->showMessage(tr("File saved"), 2000);
  */
  getMapWindow()->getCanvas()->setEnabled(true);

  return true;
}

void MapperManager::newRoomSelection(const RoomSelection*) // rs)
{
//   ActionManager *actMgr = ActionManager::self();
//   actMgr->forceRoomAct->setEnabled(FALSE);
//   _roomSelection = rs;
//   if (_roomSelection)
//   {
//     actMgr->roomActGroup->setEnabled(TRUE);
//     if (_roomSelection->size() == 1) {
//       actMgr->forceRoomAct->setEnabled(TRUE);
//     }
//   }
//   else
//   {
//     actMgr->roomActGroup->setEnabled(FALSE);
//   }
}

void MapperManager::newConnectionSelection(ConnectionSelection*)// cs)
{
//   ActionManager *actMgr = ActionManager::self();
//   _connectionSelection = cs;
//   if (_connectionSelection)
//   {
//     actMgr->connectionActGroup->setEnabled(TRUE);
//   }
//   else
//   {
//     actMgr->connectionActGroup->setEnabled(FALSE);
//   }
}

void MapperManager::onLayerUp()
{
  _mapWindow->getCanvas()->layerUp();
}

void MapperManager::onLayerDown()
{
  _mapWindow->getCanvas()->layerDown();
}

void MapperManager::onModeConnectionSelect()
{
  _mapWindow->getCanvas()->setCanvasMouseMode(MapCanvas::CMM_SELECT_CONNECTIONS);
}

void MapperManager::onModeRoomSelect()
{
  _mapWindow->getCanvas()->setCanvasMouseMode(MapCanvas::CMM_SELECT_ROOMS);
}

void MapperManager::onModeMoveSelect()
{
  _mapWindow->getCanvas()->setCanvasMouseMode(MapCanvas::CMM_MOVE);
}

void MapperManager::onModeCreateRoomSelect()
{
  _mapWindow->getCanvas()->setCanvasMouseMode(MapCanvas::CMM_CREATE_ROOMS);
}

void MapperManager::onModeCreateConnectionSelect()
{
  _mapWindow->getCanvas()->setCanvasMouseMode(MapCanvas::CMM_CREATE_CONNECTIONS);
}

void MapperManager::onModeCreateOnewayConnectionSelect()
{
  _mapWindow->getCanvas()->setCanvasMouseMode(MapCanvas::CMM_CREATE_ONEWAY_CONNECTIONS);
}

void MapperManager::onModeInfoMarkEdit()
{
  _mapWindow->getCanvas()->setCanvasMouseMode(MapCanvas::CMM_EDIT_INFOMARKS);
}

void MapperManager::onEditRoomSelection()
{
  if (_roomSelection)
  {
    RoomEditAttrDlg _roomEditDialog;
    _roomEditDialog.setRoomSelection(_roomSelection, _mapData, _mapWindow->getCanvas());
    _roomEditDialog.exec();
  }
}

void MapperManager::onEditConnectionSelection()
{

  if (_connectionSelection)
  {
    /*RoomConnectionsDlg connectionsDlg;
    connectionsDlg.setRoom(static_cast<Room*>(_connectionSelection->getFirst().room),
    _mapData,
    static_cast<Room*>(_connectionSelection->getSecond().room),
    _connectionSelection->getFirst().direction,
    _connectionSelection->getSecond().direction);
    connect(&connectionsDlg, SIGNAL(connectionChanged()), _mapWindow->getCanvas(), SLOT(update()));

    connectionsDlg.exec();
    */
  }
}

void MapperManager::onDeleteRoomSelection()
{
  if (_roomSelection)
  {
    _mapData->execute(new GroupAction(new Remove(), _roomSelection), _roomSelection);
    _mapWindow->getCanvas()->clearRoomSelection();
    _mapWindow->getCanvas()->update();
  }
}

void MapperManager::onDeleteConnectionSelection()
{
  if (_connectionSelection)
  {
    const Room *r1 = _connectionSelection->getFirst().room;
    ExitDirection dir1 = _connectionSelection->getFirst().direction;
    const Room *r2 = _connectionSelection->getSecond().room;
    ExitDirection dir2 = _connectionSelection->getSecond().direction;

    if (r2)
    {
      const RoomSelection *tmpSel = _mapData->select();
      _mapData->getRoom(r1->getId(), tmpSel);
      _mapData->getRoom(r2->getId(), tmpSel);

      _mapWindow->getCanvas()->clearConnectionSelection();

      _mapData->execute(new RemoveTwoWayExit(r1->getId(), r2->getId(), dir1, dir2), tmpSel);
      //_mapData->execute(new RemoveExit(r2->getId(), r1->getId(), dir2), tmpSel);

      _mapData->unselect(tmpSel);
    }
  }

  _mapWindow->getCanvas()->update();
}

void MapperManager::onMoveUpRoomSelection()
{
  if (!_roomSelection) return;
  Coordinate moverel(0, 0, 1);
  _mapData->execute(new GroupAction(new MoveRelative(moverel), _roomSelection), _roomSelection);
  onLayerUp();
  _mapWindow->getCanvas()->update();
}

void MapperManager::onMoveDownRoomSelection()
{
  if (!_roomSelection) return;
  Coordinate moverel(0, 0, -1);
  _mapData->execute(new GroupAction(new MoveRelative(moverel), _roomSelection), _roomSelection);
  onLayerDown();
  _mapWindow->getCanvas()->update();
}

void MapperManager::onMergeUpRoomSelection()
{
  if (!_roomSelection) return;
  Coordinate moverel(0, 0, 1);
  _mapData->execute(new GroupAction(new MergeRelative(moverel), _roomSelection), _roomSelection);
  onLayerUp();
  onModeRoomSelect();
}

void MapperManager::onMergeDownRoomSelection()
{
  if (!_roomSelection) return;
  Coordinate moverel(0, 0, -1);
  _mapData->execute(new GroupAction(new MergeRelative(moverel), _roomSelection), _roomSelection);
  onLayerDown();
  onModeRoomSelect();
}

void MapperManager::onConnectToNeighboursRoomSelection()
{
  if (!_roomSelection) return;
  _mapData->execute(new GroupAction(new ConnectToNeighbours, _roomSelection), _roomSelection);
  _mapWindow->getCanvas()->update();
}

void MapperManager::onPlayMode()
{
  QObject::disconnect(_pathMachine, SIGNAL(createRoom(ParseEvent*, const Coordinate & )), _mapData, SLOT(createRoom(ParseEvent*, const Coordinate & )));
  QObject::disconnect(_pathMachine, SIGNAL(scheduleAction(MapAction *)), _mapData, SLOT(scheduleAction(MapAction *)));
  Config().m_mapMode = 0;
}

void MapperManager::onMapMode()
{
  emit log("MainWindow","Map mode selected - new rooms are created when entering not mapped area.");
  QObject::connect(_pathMachine, SIGNAL(createRoom(ParseEvent*, const Coordinate & )), _mapData, SLOT(createRoom(ParseEvent*, const Coordinate & )));
  QObject::connect(_pathMachine, SIGNAL(scheduleAction(MapAction *)), _mapData, SLOT(scheduleAction(MapAction *)));
  Config().m_mapMode = 1;
}

void MapperManager::onOfflineMode()
{
  emit log("MainWindow","Offline emulation mode selected - you can learn areas 'safe' way.");
  QObject::disconnect(_pathMachine, SIGNAL(createRoom(ParseEvent*, const Coordinate & )), _mapData, SLOT(createRoom(ParseEvent*, const Coordinate & )));
  QObject::disconnect(_pathMachine, SIGNAL(scheduleAction(MapAction *)), _mapData, SLOT(scheduleAction(MapAction *)));
  Config().m_mapMode = 2;
}

void MapperManager::onFindRoom()
{
  if (!_findRoomsDlg) {
    _findRoomsDlg = new FindRoomsDlg(_mapData);
    _findRoomsDlg->setObjectName("FindRoomsDlg");

    connect(_findRoomsDlg, SIGNAL(log( const QString&, const QString& )),
	    _eventHandler, SLOT(log( const QString&, const QString& )));
    connect(_findRoomsDlg, SIGNAL(center(qint32, qint32)),
	    _mapWindow, SLOT(center(qint32, qint32)));

    qDebug("FindRooms loaded");

  }
  _findRoomsDlg->show();
}
