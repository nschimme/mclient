#include <QApplication>
#include <QDebug>

#include "MultiLineInput.h"
#include "EventHandler.h"
#include "InputWidget.h"
#include "CommandEntry.h"

Q_EXPORT_PLUGIN2(multilineinput, MultiLineInput)


MultiLineInput::MultiLineInput(QWidget* parent) 
        : MClientDisplayPlugin(parent) {
    _shortName = "multilineinput";
    _longName = "Multi Line Input";
    _description = "A multi lined input plugin.";
    //_dependencies.insert("terrible_test_api", 1);
//    _implemented.insert("some_other_api",1);
    _receivesDataTypes << "CommandHistory" << "EchoMode"
		       << "SocketDisconnected";
    //_deliversDataTypes << "UserInput";
    _configurable = false;
    _configVersion = "2.0";

    // Allowable Display Locations
    SET(_displayLocations, DL_INPUT);

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


bool MultiLineInput::loadSettings() {

  return true;
}

        
bool MultiLineInput::saveSettings() const {
  return true;
}


bool MultiLineInput::startSession(QString s) {
  _eventHandlers[s] = new EventHandler;
  return true;
}


bool MultiLineInput::stopSession(QString s) {
  if (_widgets[s]->close()) {
    qDebug() << "* removed MultiLineInput InputWidget for session" << s;
    delete _eventHandlers[s];
  }
  return true;
}


MClientEventHandler* MultiLineInput::getEventHandler(QString s) {
  return _eventHandlers[s].data();
}


// Display plugin members
bool MultiLineInput::initDisplay(QString s) {
  _widgets[s] = new InputWidget(s, this);  

  // Connect Signals/Slots
  connect(_widgets[s], SIGNAL(sendUserInput(const QString&, bool)),
	  _eventHandlers[s], SLOT(sendUserInput(const QString&, bool)));
  connect(_widgets[s], SIGNAL(displayMessage(const QString &)),
	  _eventHandlers[s], SLOT(displayMessage(const QString &)));
  connect(_eventHandlers[s], SIGNAL(setEchoMode(bool)),
	  _widgets[s], SLOT(toggleEchoMode(bool)));
  connect(_eventHandlers[s], SIGNAL(showCommandHistory()),
	  _widgets[s], SLOT(showCommandHistory()));

  return true;
}

QWidget* MultiLineInput::getWidget(QString s) {
  return _widgets[s];
}
