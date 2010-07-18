/***************************************************************************
 *   Copyright (C) 2008 by Nils Schimmelmann   *
 *   nschimme@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 ***************************************************************************/

#ifndef _MAPPERMANAGER_H_
#define _MAPPERMANAGER_H_

#include <QProgressDialog>

#include <QPointer>
#include <QThread>

class Configuration;

class MapWindow;
class Mmapper2PathMachine;
class CommandEvaluator;
class PrespammedPath;
class MapData;
class RoomSelection;
class ConnectionSelection;
class RoomPropertySetter;
class FindRoomsDlg;
class ConfigDialog;

class EventHandler;
class MMapperPluginParser;
class MainWindow;

class MapperManager: public QObject {
  Q_OBJECT

  public:
    MapperManager(EventHandler *);
    ~MapperManager();

    bool initDisplay(QWidget *parent);

    MapWindow *getMapWindow() { return _mapWindow; }
    Mmapper2PathMachine *getPathMachine() { return _pathMachine; }
    MapData *getMapData() { return _mapData; }
    PrespammedPath *getPrespammedPath() { return _prespammedPath; }
    MMapperPluginParser* getParser() { return _parser; }
    FindRoomsDlg *getFindRoomsDlg() { return _findRoomsDlg; }

    bool maybeSave();

  public slots:
    void newFile();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName, bool =false);
    void open();
    void reload();
    void merge();
    bool save();
    bool saveAs();

    //void setCurrentFile(const QString &fileName);
    void percentageChanged(quint32);

    void onModeConnectionSelect();
    void onModeRoomSelect();
    void onModeMoveSelect();
    void onModeInfoMarkEdit();
    void onModeCreateRoomSelect();
    void onModeCreateConnectionSelect();
    void onModeCreateOnewayConnectionSelect();
    void onLayerUp();
    void onLayerDown();
    void onEditRoomSelection();
    void onDeleteRoomSelection();
    void onDeleteConnectionSelection();
    void onMoveUpRoomSelection();
    void onMoveDownRoomSelection();
    void onMergeUpRoomSelection();
    void onMergeDownRoomSelection();
    void onConnectToNeighboursRoomSelection();
    void onPlayMode();
    void onMapMode();
    void onOfflineMode();
    void onFindRoom();
    void onPreferences();

    void newRoomSelection(const RoomSelection*);
    void newConnectionSelection(ConnectionSelection*);

  signals:
    void log(const QString&, const QString&);
    void initMapCanvas(MapData*, PrespammedPath*);

  private:
    MapWindow *_mapWindow;
    Mmapper2PathMachine *_pathMachine;
    MapData *_mapData;
    RoomPropertySetter *_propertySetter;
    PrespammedPath *_prespammedPath;
    MMapperPluginParser *_parser;

    const RoomSelection *_roomSelection;
    ConnectionSelection *_connectionSelection;

    QPointer<QProgressDialog> progressDlg;
    FindRoomsDlg *_findRoomsDlg;

    EventHandler *_eventHandler;
    QPointer<MainWindow> _mainWindow;
};

#endif /* _MAPPERMANAGER_H_ */
