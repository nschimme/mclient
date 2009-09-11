#include "SocketManagerIO.h"
#include "SocketManagerIOConfig.h"
#include "SocketReader.h"
#include "EventHandler.h"

#include "PluginManager.h"
#include "PluginSession.h"
#include "CommandProcessor.h"
#include "CommandEntry.h"
#include "ConfigManager.h"

#include <QDebug>

Q_EXPORT_PLUGIN2(socketmanagerio, SocketManagerIO)


SocketManagerIO::SocketManagerIO(QObject* parent) 
        : MClientPlugin(parent) {
    
    _shortName = "socketmanagerio";
    _longName = "SocketManager";
    _description = "A socket plugin that reads from sockets and inserts the data back into the stream.";
    //    _dependencies.insert("commandmanager", 10);
    _implemented.insert("socketmanager",10);
    _receivesDataTypes << "SendToSocketData" << "ConnectToHost"
		       << "DisconnectFromHost"
		       << "MUMEPromptGARequest";
    /*
    _deliversDataTypes << "SocketReadData" << "SocketConnected"
		       << "SocketDisconnected" << "DisplayData";
    */
    _configurable = true;
    _configVersion = "2.0";

    // Command: connect
    CommandEntry *connect = new CommandEntry();
    connect->pluginName(shortName());
    connect->command("connect");
    connect->help("connect to the host");
    connect->dataType("ConnectToHost");

    // Command: zap
    CommandEntry *zap = new CommandEntry();
    zap->pluginName(shortName());
    zap->command("zap");
    zap->help("disconnect from the host");
    zap->dataType("DisconnectFromHost");


    // Command: send
    CommandEntry *send = new CommandEntry();
    send->pluginName(shortName());
    send->command("send");
    send->help("send data directly to the host");
    send->dataType("SendToSocketData");

    // Command: prompt
    CommandEntry *prompt = new CommandEntry();
    prompt->pluginName(shortName());
    prompt->command("prompt");
    prompt->help("send the MUME prompt request");
    prompt->dataType("MUMEPromptGARequest");
    
    // For registering commands
    _commandEntries << connect << send << prompt << zap;
}


SocketManagerIO::~SocketManagerIO() {
  // Clean up the QHash of sockets.
  // saveSettings();
}


void SocketManagerIO::configure() {
    // Need to display a table of (identifier, host, port)
    // Selecting one and pushing an "Edit..." button will bring up a dialog
    // with three QLineEdits, one for each field.  Closing that will save the
    // data to a member variable and update the table.
    
    // It will originally be populated by QSettings.

    if(!_configWidget) _configWidget = new SocketManagerIOConfig();
    if(!_configWidget->isVisible()) _configWidget->show();

}


bool SocketManagerIO::loadSettings() {
  /*
  _settings =
    _pluginManager->getConfig()->pluginSettings(_pluginSession->session(),
						_shortName);
  */
  
  return true;
}


bool SocketManagerIO::saveSettings() const {
  /*
  _plugiManager->getConfig()->writePluginSettings(_pluginSession->session(),
				     _shortName);
  */
  return true;
}


bool SocketManagerIO::startSession(QString s) {
  _settings = new QHash<QString, QString>;

    QString cfg = QString("config/%1/").arg(s);

    // Host settings
    QString host = _settings->value(cfg+"connection/host", "mume.org");
    int port = _settings->value(cfg+"connection/port", "4242").toInt();

    //host = "127.0.0.1";

    // Proxy settings
    QString proxy_host = _settings->value(cfg+"proxy/host", "proxy.example.com");
    int proxy_port = _settings->value(cfg+"proxy/port", "0").toInt();
    QString proxy_user = _settings->value(cfg+"proxy/proxy_user", "");
    QString proxy_pass = _settings->value(cfg+"proxy/proxy_pass", "");


    _socketReaders[s] = new SocketReader(s, this);
    if(proxy_port != 0 && !proxy_host.isEmpty()) {
        QNetworkProxy* proxy = new QNetworkProxy();
        //proxy->setType(QNetworkProxy::Socks5Proxy);
        proxy->setHostName(proxy_host);
        proxy->setPort(proxy_port);
        proxy->setUser(proxy_user);
        proxy->setPassword(proxy_pass);

        _socketReaders[s]->proxy(proxy);
        qDebug() << "* added proxy" << proxy_host << proxy_port
		 << "to SocketReader in session" << s;
    }
    _socketReaders[s]->host(host);
    _socketReaders[s]->port(port);

  _eventHandlers[s] = new EventHandler;

  connect(_eventHandlers[s], SIGNAL(connectToHost()),
	  _socketReaders[s], SLOT(connectToHost()));
  connect(_eventHandlers[s], SIGNAL(closeSocket()),
	  _socketReaders[s], SLOT(closeSocket()));
  connect(_eventHandlers[s], SIGNAL(sendToSocket(const QByteArray &)),
	  _socketReaders[s], SLOT(sendToSocket(const QByteArray &)));

  connect(_socketReaders[s], SIGNAL(socketReadData(const QByteArray &)),
	  _eventHandlers[s], SLOT(socketReadData(const QByteArray &)));
  connect(_socketReaders[s], SIGNAL(displayMessage(const QString &)),
	  _eventHandlers[s], SLOT(displayMessage(const QString &)));
  connect(_socketReaders[s], SIGNAL(socketOpened()),
	  _eventHandlers[s], SLOT(socketOpened()));
  connect(_socketReaders[s], SIGNAL(socketClosed()),
	  _eventHandlers[s], SLOT(socketClosed()));

  return true;
}


bool SocketManagerIO::stopSession(QString s) {
  delete _socketReaders[s];
  delete _eventHandlers[s];
  qDebug() << "* removed SocketReader for session" << s;
  return true;
}


MClientEventHandler* SocketManagerIO::getEventHandler(QString s) {
  if (_eventHandlers.contains(s))
      return _eventHandlers[s].data();
  else {
      qDebug() << "! SocketManagerIO unable to find EventHandler for"
               << s << "; hash is" << _eventHandlers;
      return 0;
  }
}
