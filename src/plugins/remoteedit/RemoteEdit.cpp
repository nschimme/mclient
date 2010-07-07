#include "RemoteEdit.h"

#include "AbstractPluginSession.h"
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
    _receivesDataTypes.insert("SocketConnected", 3);
    _receivesDataTypes.insert("XMLEdit", 3);
    _receivesDataTypes.insert("XMLEditTitle", 3);
    _receivesDataTypes.insert("XMLEditBody", 3);
    _receivesDataTypes.insert("XMLView", 3);
    _receivesDataTypes.insert("XMLViewTitle", 3);
    _receivesDataTypes.insert("XMLViewBody", 3);
    _receivesDataTypes.insert("MUMEIdentifyRequest", 3);

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


bool RemoteEdit::startSession(AbstractPluginSession *ps) {
  _eventHandlers[ps->session()] = new EventHandler(ps, this);
  return true;
}


bool RemoteEdit::stopSession(const QString &session) {
  _eventHandlers[session]->deleteLater();
  _eventHandlers.remove(session);
  return true;
}


MClientEventHandler* RemoteEdit::getEventHandler(const QString &s) {
  return _eventHandlers[s].data();
}
