#include "MMapperPlugin.h"

#include "CommandEntry.h"
#include "EventHandler.h"
#include "AbstractPluginSession.h"

#include <QDebug>

Q_EXPORT_PLUGIN2(mmapperplugin, MMapperPlugin)


MMapperPlugin::MMapperPlugin(QObject* parent) 
        : MClientPlugin(parent) {
    
    _shortName = "mmapperplugin";
    _longName = "MMapper Plugin";
    _description = "An implementation of MMapper as a plugin for mClient";
    //_dependencies.insert("commandmanager", 10);
    //_implemented.insert("socketmanager",10);
    _receivesDataTypes.insert("XMLAll", 3);
    _receivesDataTypes.insert("XMLMove", 3);
    _receivesDataTypes.insert("MMapperLoadMap", 3);
    _receivesDataTypes.insert("MMapperInput", 3);
    _receivesDataTypes.insert("SocketConnected", 3);
    _receivesDataTypes.insert("SocketDisconnected", 3);
    //_deliversDataTypes << "DisplayData";
    _configurable = false;
    _configVersion = "2.0";

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


bool MMapperPlugin::startSession(AbstractPluginSession *ps) {
  _eventHandlers[ps->session()] = new EventHandler(ps, this);
  return true;
}


bool MMapperPlugin::stopSession(const QString &session) {
  _eventHandlers[session]->deleteLater();
  _eventHandlers.remove(session);
  qDebug() << "* removed MapperManager for session" << session;
  return true;
}


MClientEventHandler* MMapperPlugin::getEventHandler(const QString &s) {
  return _eventHandlers[s].data();
}
