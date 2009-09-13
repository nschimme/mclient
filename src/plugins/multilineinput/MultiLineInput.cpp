#include <QApplication>
#include <QDebug>

#include "MultiLineInput.h"

#include "EventHandler.h"
#include "CommandEntry.h"
#include "PluginSession.h"

Q_EXPORT_PLUGIN2(multilineinput, MultiLineInput)


MultiLineInput::MultiLineInput(QObject* parent) 
        : MClientPlugin(parent) {
    _shortName = "multilineinput";
    _longName = "Multi Line Input";
    _description = "A multi-lined input plugin.";
    //_dependencies.insert("terrible_test_api", 1);
//    _implemented.insert("some_other_api",1);
    _receivesDataTypes << "CommandHistory" << "EchoMode"
		       << "SocketDisconnected";
    //_deliversDataTypes << "UserInput";
    _configurable = false;
    _configVersion = "2.0";

    // Command: history
    CommandEntry *history = new CommandEntry();
    history->pluginName(shortName());
    history->command("history");
    history->help("show command history");
    history->dataType("CommandHistory");
    
    // For registering commands
    _commandEntries << history;
}


MultiLineInput::~MultiLineInput() {
}


void MultiLineInput::configure() {
}


bool MultiLineInput::startSession(PluginSession *ps) {
  _eventHandlers[ps->session()] = new EventHandler(ps, this);
  return true;
}


bool MultiLineInput::stopSession(PluginSession *ps) {
  qDebug() << "* removed MultiLineInput InputWidget for session"
	   << ps->session();
  delete _eventHandlers[ps->session()];
  return true;
}


MClientEventHandler* MultiLineInput::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}
