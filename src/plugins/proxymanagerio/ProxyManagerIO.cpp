#include "ProxyManagerIO.h"
#include "EventHandler.h"
#include "CommandEntry.h"

Q_EXPORT_PLUGIN2(proxymanagerio, ProxyManagerIO)


ProxyManagerIO::ProxyManagerIO(QObject* parent) : MClientPlugin(parent) {
  _shortName = "proxymanagerio";
  _longName = "ProxyManager";
  _description = "A proxy plugin";
  //    _dependencies.insert("commandmanager", 10);
  _implemented.insert("proxymanager",10);
  _receivesDataTypes << "DisplayData" << "UserInput"
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

bool ProxyManagerIO::loadSettings() {
  /*
    _settings =
    _pluginManager->getConfig()->pluginSettings(_pluginSession->session(),
    _shortName);
  */
  
return true;
}


bool ProxyManagerIO::saveSettings() const {
  /*
    _plugiManager->getConfig()->writePluginSettings(_pluginSession->session(),
    _shortName);
  */
  return true;
}


bool ProxyManagerIO::startSession(QString s) {
  _eventHandlers[s] = new EventHandler;

  /*
  _settings = new QHash<QString, QString>;

  QString cfg = QString("config/%1/").arg(s);

  // Host settings
  QString host = _settings->value(cfg+"connection/host", "mume.org");
  int port = _settings->value(cfg+"connection/port", "4242").toInt();

  // Proxy settings
  QString proxy_host = _settings->value(cfg+"proxy/host", "proxy.example.com");
  int proxy_port = _settings->value(cfg+"proxy/port", "0").toInt();
  QString proxy_user = _settings->value(cfg+"proxy/proxy_user", "");
  QString proxy_pass = _settings->value(cfg+"proxy/proxy_pass", "");

  if(proxy_port != 0 && !proxy_host.isEmpty()) {
    QNetworkProxy* proxy = new QNetworkProxy();
    //proxy->setType(QNetworkProxy::Socks5Proxy);
    proxy->setHostName(proxy_host);
    proxy->setPort(proxy_port);
    proxy->setUser(proxy_user);
    proxy->setPassword(proxy_pass);

    _proxyServers[s]->proxy(proxy);
    qDebug() << "* added proxy" << proxy_host << proxy_port
	     << "to ProxyServer in session" << s;
  }
  _proxyServers[s]->host(host);
  _proxyServers[s]->port(port);
  */

  return true;
}


bool ProxyManagerIO::stopSession(QString s) {
  delete _eventHandlers[s];
  return true;
}


MClientEventHandler* ProxyManagerIO::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}
