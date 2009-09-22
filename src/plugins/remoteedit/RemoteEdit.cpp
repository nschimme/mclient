#include "RemoteEdit.h"

#include "PluginSession.h"
#include "EventHandler.h"
#include "CommandEntry.h"

#include <QDebug>

Q_EXPORT_PLUGIN2(remoteedit, RemoteEdit)


RemoteEdit::RemoteEdit(QObject* parent) 
        : MClientPlugin(parent) {
    _shortName = "remoteedit";
    _longName = "Mume Remote Editing Protocol";
    _description = "MUME Remote Editing protocol";
    _dependencies.insert("telnet", 1);
    _implemented.insert("remoteedit",1);
    _receivesDataTypes << "XMLEdit" << "XMLEditTitle" << "XMLEditBody"
		       << "XMLView" << "XMLViewTitle" << "XMLViewBody"
		       << "MUMEIdentifyRequest" << "SocketConnected";

    // Command: identify
    CommandEntry *identify = new CommandEntry();
    identify->pluginName(shortName());
    identify->command("identify");
    identify->help("send MUME client identification request");
    identify->dataType("MUMEIdentifyRequest");
    
    // For registering commands
    _commandEntries << identify;

}


RemoteEdit::~RemoteEdit() {
}


void RemoteEdit::configure() {
}


bool RemoteEdit::startSession(PluginSession *ps) {
  _eventHandlers[ps->session()] = new EventHandler(ps, this);
  return true;
}


bool RemoteEdit::stopSession(PluginSession *ps) {
  delete _eventHandlers[ps->session()];
  return true;
}


MClientEventHandler* RemoteEdit::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}
