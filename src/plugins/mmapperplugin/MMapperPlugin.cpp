#include "MMapperPlugin.h"

#include "MapperManager.h"

#include "MClientEvent.h"
#include "PluginManager.h"
#include "PluginSession.h"
#include "CommandManager.h"
#include "ConfigManager.h"

#include "mapwindow.h" // for grabbing the QWidget

#include <QApplication>

Q_EXPORT_PLUGIN2(mmapperplugin, MMapperPlugin)


MMapperPlugin::MMapperPlugin(QWidget* parent) 
        : MClientDisplayPlugin(parent) {
    
    _shortName = "mmapperplugin";
    _longName = "MMapper Plugin";
    _description = "An implementation of MMapper as a plugin for mClient";
    //_dependencies.insert("commandmanager", 10);
    //_implemented.insert("socketmanager",10);
    _receivesDataTypes << "MMapperInput"
		       << "XMLName" << "XMLDescription"
		       << "XMLDynamicDescription" << "XMLExits" << "XMLPrompt"
		       << "XMLMove" << "XMLNone" 
		       << "MMapperLoadMap";
    _deliversDataTypes << "DisplayData";
    _configurable = false;
    _configVersion = "2.0";

    // Allowable Display Locations
    SET(_displayLocations, DL_FLOAT);
}


MMapperPlugin::~MMapperPlugin() {
    //saveSettings();
}

// MClientPlugin members
void MMapperPlugin::customEvent(QEvent* e) {
  if(e->type() == 10000)
    engineEvent(e);
  else {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    foreach(QString s, me->dataTypes()) {
      if (s.startsWith("X")) {
	if (s.startsWith("XMLNone")) {
	  emit mudOutput(me->payload()->toString());
	  
	} else if (s.startsWith("XMLName")) {
	  emit name(me->payload()->toString());
	  
	} else if (s.startsWith("XMLDescription")) {
	  emit description(me->payload()->toString());
	  
	} else if (s.startsWith("XMLDynamicDescription")) {
	  emit dynamicDescription(me->payload()->toString());
	  
	} else if (s.startsWith("XMLExits")) {
	  emit exits(me->payload()->toString());
      
	} else if (s.startsWith("XMLPrompt")) {
	  emit prompt(me->payload()->toString());

	} else if (s.startsWith("XMLMove")) {
	  emit move(me->payload()->toString());
	  
	}

      }
      else if (s.startsWith("M")) {
	if(s.startsWith("MMapperInput")) {
	  emit userInput(me->payload()->toString());
	  
	} else if (s.startsWith("MMapperLoadMap")) {
	  emit loadFile("/home/nschimme/arda.mm2");
	  
	}
      }
      
    }
  }
}


void MMapperPlugin::configure() {
}


bool MMapperPlugin::loadSettings() {
    _settings = _pluginSession->getManager()->getConfig()->pluginSettings(_shortName);

    // register commands
    QStringList commands;
    commands << _shortName
	     << "map" << "MMapperLoadMap"
	     << "input" << "MMapperInput";
    _pluginSession->getManager()->getCommand()->registerCommand(commands);

    return true;
}


bool MMapperPlugin::saveSettings() const {
    _pluginSession->getManager()->getConfig()->writePluginSettings(_shortName);
    return true;
}


bool MMapperPlugin::startSession(QString /* s */) {   
  return true;
}


bool MMapperPlugin::stopSession(QString s) {
  _mapper->getMapWindow()->close();
  delete _mapper;
  qDebug() << "* removed MapperManager for session" << s;
  return true;
}

// Display plugin members
bool MMapperPlugin::initDisplay(QString) {
  _mapper = new MapperManager(_session, this);
  _mapper->start(LowPriority);
  
  connect(this, SIGNAL(loadFile(const QString&)),
	  _mapper, SLOT(loadFile(const QString&)));

    return true;
}


QWidget* MMapperPlugin::getWidget(QString) {
  return _mapper->getMapWindow();
}


void MMapperPlugin::displayMessage(const QString& message) {
  QVariant* qv = new QVariant(message);
  QStringList sl("DisplayData");
  postSession(qv, sl);
}

void MMapperPlugin::log(const QString& message, const QString& s) {
  qDebug() << "* MMapperPlugin[" << s << "]: " << message;
}
