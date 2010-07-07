#include "TellMessenger.h"
#include "TellMessengerConfig.h"
#include "EventHandler.h"

#include "PluginManager.h"
#include "AbstractPluginSession.h"
#include "CommandProcessor.h"
#include "CommandEntry.h"
#include "ConfigManager.h"

#include <QDebug>

Q_EXPORT_PLUGIN2(tellmessenger, TellMessenger)


TellMessenger::TellMessenger(QObject* parent) 
        : MClientPlugin(parent) {
    
    _shortName = "tellmessenger";
    _longName = "Tell Messenger";
    _description = "A plugin that listens for XML tell tags and sends them out over a socket.";
    //    _dependencies.insert("commandmanager", 10);
    _implemented.insert("tellmessenger",10);
    _receivesDataTypes.insert("XMLTell", 3);
    /*
    _deliversDataTypes << "SocketReadData" << "SocketConnected"
		       << "SocketDisconnected" << "DisplayData";
    */
    _configurable = true;
    _configVersion = "1.0";

    /*
    // Command: connect
    CommandEntry *connect = new CommandEntry();
    connect->pluginName(shortName());
    connect->command("connect");
    connect->help("connect to the host");
    connect->dataType("ConnectToHost");
    */

    // For registering commands
    //_commandEntries << connect;
}


TellMessenger::~TellMessenger() {
}


void TellMessenger::configure() {
    // Need to display a table of (identifier, host, port)
    // Selecting one and pushing an "Edit..." button will bring up a dialog
    // with three QLineEdits, one for each field.  Closing that will save the
    // data to a member variable and update the table.
    
    // It will originally be populated by QSettings.

//    if(!_configWidget) _configWidget = new TellMessengerConfig();
//    if(!_configWidget->isVisible()) _configWidget->show();

}


bool TellMessenger::startSession(AbstractPluginSession *ps) {
  QString s = ps->session();
  _eventHandlers[s] = new EventHandler(ps, this);
  return true;
}


bool TellMessenger::stopSession(const QString &session) {
  _eventHandlers[session]->deleteLater();
  _eventHandlers.remove(session);
  qDebug() << "* removed SocketServer for session" << session;
  return true;
}


MClientEventHandler* TellMessenger::getEventHandler(const QString &s) {
  if (_eventHandlers.contains(s))
      return _eventHandlers[s].data();
  else {
      qDebug() << "! TellMessenger unable to find EventHandler for"
               << s << "; hash is" << _eventHandlers;
      return 0;
  }
}
