#include "ProxyManagerIO.h"
#include "EventHandler.h"
#include "CommandEntry.h"
#include "PluginSession.h"

Q_EXPORT_PLUGIN2(proxymanagerio, ProxyManagerIO)


ProxyManagerIO::ProxyManagerIO(QObject* parent) : MClientPlugin(parent) {
  _shortName = "proxymanagerio";
  _longName = "ProxyManager";
  _description = "A proxy plugin";
  //    _dependencies.insert("commandmanager", 10);
  _implemented.insert("proxymanager",10);
  _receivesDataTypes << "DisplayData" << "DisplayPrompt" << "UserInput"
		     << "EchoMode" << "SocketDisconnected"
		     << "ProxyCommand";
  // _deliversDataTypes << "SendToUser";
  _configurable = false;

  // Command: proxy
  CommandEntry *proxycmd = new CommandEntry();
  proxycmd->pluginName(shortName());
  proxycmd->command("proxy");
  proxycmd->help("list/kill proxy connections");
  proxycmd->dataType("ProxyCommand");
  
  // For registering commands
  _commandEntries << proxycmd;

}


ProxyManagerIO::~ProxyManagerIO() {
}


void ProxyManagerIO::configure() {
}


bool ProxyManagerIO::startSession(PluginSession *ps) {
  _eventHandlers[ps->session()] = new EventHandler(ps, this);
  return true;
}


bool ProxyManagerIO::stopSession(PluginSession *ps) {
  _eventHandlers[ps->session()]->deleteLater();
  _eventHandlers.remove(ps->session());
  return true;
}


MClientEventHandler* ProxyManagerIO::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}
