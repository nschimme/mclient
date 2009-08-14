#include "MMapperPlugin.h"

/*
#include "MClientEvent.h"
#include "PluginManager.h"
#include "PluginSession.h"
#include "CommandProcessor.h"
#include "ConfigManager.h"
*/
#include "CommandEntry.h"

#include "EventHandler.h"
#include "MapperManager.h"
#include "MMapperPluginParser.h"
#include "mapwindow.h" // for grabbing the QWidget

#include <QApplication>

Q_EXPORT_PLUGIN2(mmapperplugin, MMapperPlugin)


MMapperPlugin::MMapperPlugin(QWidget* parent) 
        : MClientDisplayPlugin(parent) {
    
    _shortName = "mmapperplugin";
    _longName = "MMapper Plugin";
    _description = "An implementation of MMapper as a plugin for mClient";
    //_dependencies.insert("commandmanager", 10);
    //_implemented.insert("socketmanager",10);
    _receivesDataTypes << "XMLName" << "XMLDescription"
		       << "XMLDynamicDescription" << "XMLExits" << "XMLPrompt"
		       << "XMLMove" << "XMLNone" << "XMLTerrain"
		       << "MMapperLoadMap" << "MMapperInput"
		       << "SocketConnected" << "SocketDisconnected";
    _deliversDataTypes << "DisplayData";
    _configurable = false;
    _configVersion = "2.0";

    // Allowable Display Locations
    SET(_displayLocations, DL_FLOAT);

    // Command: loadmap
    CommandEntry *loadmap = new CommandEntry();
    loadmap->pluginName(shortName());
    loadmap->command("loadmap");
    loadmap->help("load a mmapper2 map file");
    loadmap->dataType("MMapperLoadMap");

    // Command: offline
    CommandEntry *mappercmd = new CommandEntry();
    mappercmd->pluginName(shortName());
    mappercmd->command("mapper");
    mappercmd->help("run a MMapper command");
    mappercmd->dataType("MMapperInput");

    // For registering commands
    _commandEntries << loadmap << mappercmd;

}


MMapperPlugin::~MMapperPlugin() {
}


void MMapperPlugin::configure() {
}


bool MMapperPlugin::loadSettings() {
    return true;
}


bool MMapperPlugin::saveSettings() const {
    return true;
}


bool MMapperPlugin::startSession(QString s) {
  _eventHandlers[s] = new EventHandler;
  return true;
}


bool MMapperPlugin::stopSession(QString s) {
  _eventHandlers[s]->deleteLater();
  _mappers[s]->getMapWindow()->close();
  _mappers[s]->deleteLater();
  qDebug() << "* removed MapperManager for session" << s;
  return true;
}

// Display plugin members
bool MMapperPlugin::initDisplay(QString s) {
  _mappers[s] = new MapperManager(_eventHandlers[s]);
  _mappers[s]->start();

  return true;
}


QWidget* MMapperPlugin::getWidget(QString s) {
  return _mappers[s]->getMapWindow();
}


MClientEventHandler* MMapperPlugin::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}
