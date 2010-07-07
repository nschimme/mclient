#include "ProxyManagerIO.h"
#include "EventHandler.h"
#include "CommandEntry.h"
#include "AbstractPluginSession.h"

Q_EXPORT_PLUGIN2(proxymanagerio, ProxyManagerIO)


ProxyManagerIO::ProxyManagerIO(QObject* parent) : MClientPlugin(parent) {
  _shortName = "proxymanagerio";
  _longName = "ProxyManager";
  _description = "A proxy plugin";
  //    _dependencies.insert("commandmanager", 10);
  _implemented.insert("proxymanager",10);
  _receivesDataTypes.insert("DisplayData", 3);
  _receivesDataTypes.insert("DisplayPrompt", 3);
  _receivesDataTypes.insert("UserInput", 3);
  _receivesDataTypes.insert("EchoMode", 3);
  _receivesDataTypes.insert("SocketDisconnected", 3);
  _receivesDataTypes.insert("ProxyCommand", 3);
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


bool ProxyManagerIO::startSession(AbstractPluginSession *ps) {
  _eventHandlers[ps->session()] = new EventHandler(ps, this);
  return true;
}


bool ProxyManagerIO::stopSession(const QString &session) {
  _eventHandlers[session]->deleteLater();
  _eventHandlers.remove(session);
  return true;
}


MClientEventHandler* ProxyManagerIO::getEventHandler(const QString &s) {
  return _eventHandlers[s].data();
}
