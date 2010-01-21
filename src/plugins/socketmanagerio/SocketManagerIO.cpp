#include "SocketManagerIO.h"
#include "SocketManagerIOConfig.h"
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
		       << "MUMEPromptGARequest"
		       << "DoneLoading";
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
  qDebug() << "~SocketManagerIO";
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


bool SocketManagerIO::startSession(PluginSession *ps) {
  QString s = ps->session();
  _eventHandlers[s] = new EventHandler(ps, this);
  return true;
}


bool SocketManagerIO::stopSession(PluginSession *ps) {
  QString s = ps->session();
  _eventHandlers[ps->session()]->deleteLater();
  _eventHandlers.remove(ps->session());
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
