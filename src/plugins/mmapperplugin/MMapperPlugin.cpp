#include "MMapperPlugin.h"

#include "MapperManager.h"

#include "MClientEvent.h"
#include "PluginManager.h"
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
    _dataTypes << "MMapperInput"
	       << "XMLName" << "XMLDescription" << "XMLDynamicDescription"
	       << "XMLExits" << "XMLPrompt" << "XMLMove" << "XMLNone"
	       << "MMapperLoadMap";
    _configurable = false;
    _configVersion = "2.0";

    // Allowable Display Locations
    SET(_displayLocations, DL_FLOAT);

    // register commands
    QStringList commands;
    commands << _shortName
	     << "map" << "MMapperLoadMap"
	     << "input" << "MMapperInput";
    CommandManager::instance()->registerCommand(commands);

}


MMapperPlugin::~MMapperPlugin() {
    stopAllSessions();
    saveSettings();
}

// MClientPlugin members
void MMapperPlugin::customEvent(QEvent* e) {
    if(!e->type() == 10001) return;
    
    MClientEvent* me = static_cast<MClientEvent*>(e);

    foreach(QString s, me->dataTypes()) {
      if (s.startsWith("X")) {
	if (s.startsWith("XMLNone")) {
	  emit mudOutput(me->payload()->toString(), me->session());
	  
	} else if (s.startsWith("XMLName")) {
	  emit name(me->payload()->toString(), me->session());
	  
	} else if (s.startsWith("XMLDescription")) {
	  emit description(me->payload()->toString(), me->session());
	  
	} else if (s.startsWith("XMLDynamicDescription")) {
	  emit dynamicDescription(me->payload()->toString(), me->session());
	  
	} else if (s.startsWith("XMLExits")) {
	  emit exits(me->payload()->toString(), me->session());
      
	} else if (s.startsWith("XMLPrompt")) {
	  emit prompt(me->payload()->toString(), me->session());

	} else if (s.startsWith("XMLMove")) {
	  emit move(me->payload()->toString(), me->session());
	  
	}

      }
      else if (s.startsWith("M")) {
	if(s.startsWith("MMapperInput")) {
	  emit userInput(me->payload()->toString(), me->session());
	  
	} else if (s.startsWith("MMapperLoadMap")) {
	  emit loadFile("/home/nschimme/arda.mm2");
	  
	}
      }
      
    }
}


void MMapperPlugin::configure() {
}


const bool MMapperPlugin::loadSettings() {
    _settings = ConfigManager::instance()->pluginSettings(_shortName);
    return true;
}


const bool MMapperPlugin::saveSettings() const {
    ConfigManager::instance()->writePluginSettings(_shortName);
    return true;
}


const bool MMapperPlugin::startSession(QString s) {
    MapperManager *mm = new MapperManager(s, this);
    mm->start(LowPriority);

    connect(this, SIGNAL(loadFile(const QString&)),
	    mm, SLOT(loadFile(const QString&)));
    
    _mappers.insert(s, mm);
    _runningSessions << s;
    return true;
}


const bool MMapperPlugin::stopSession(QString s) {
    foreach(MapperManager *mm, _mappers.values(s)) {
        mm->getMapWindow()->close();
        delete mm;
        qDebug() << "* removed MapperManager for session" << s;
    }
    _mappers.remove(s);
    int removed = _runningSessions.removeAll(s);
    return removed!=0?true:false;
}

// Display plugin members
const bool MMapperPlugin::initDisplay(QString s) {
    return true;
}


QWidget* MMapperPlugin::getWidget(QString s) {
  return _mappers[s]->getMapWindow();
}


void MMapperPlugin::displayMessage(const QString& message, const QString& s) {
    QVariant* qv = new QVariant(message);
    QStringList sl("DisplayData");
    postEvent(qv, sl, s);
}

void MMapperPlugin::log(const QString& message, const QString& s) {
  qDebug() << "* MMapperPlugin[" << s << "]: " << message;
}
