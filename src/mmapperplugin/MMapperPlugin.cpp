#include "MMapperPlugin.h"

#include "MMapperPluginParser.h"
#include "MapperManager.h"

#include "MClientEvent.h"
#include "PluginManager.h"
#include "CommandManager.h"
#include "ConfigManager.h"

#include "mapwindow.h" // for grabbing the QWidget

#include <QApplication>

Q_EXPORT_PLUGIN2(mmapperplugin, MMapperPlugin)


enum MMapperPluginEventType { XML_NONE = 0, XML_NAME, XML_DESCRIPTION,
			      XML_DYNAMIC_DESCRIPTION, XML_EXITS, XML_PROMPT,
			      XML_MOVE, MMAPPER_INPUT, MMAPPER_LOAD_MAP };

struct EventData {
  MMapperPluginEventType type;
  QString data;
  QString session;
};


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
	  EventData *foo = new EventData;
	  foo->data = me->payload()->toString();
	  foo->session = me->session();
	  foo->type = MMAPPER_INPUT;
	  _eventQueue.enqueue(foo);
	  if(!isRunning()) {
	    start(LowPriority);
	  }
	  //emit userInput(me->payload()->toString(), me->session());
	  
	} else if (s.startsWith("MMapperLoadMap")) {
// 	  EventData *foo = new EventData;
// 	  foo->data = me->payload()->toString();
// 	  foo->session = me->session();
// 	  foo->type = MMAPPER_LOAD_MAP;
// 	  _eventQueue.enqueue(foo);
// 	  if(!isRunning()) start(LowPriority);
	  emit loadFile("/home/nschimme/arda.mm2");
	  
	}
      }
      
    }
}


void MMapperPlugin::run() {
  while(_eventQueue.count() > 0) {
    EventData *foo = _eventQueue.dequeue();

    switch (foo->type) {
    case MMAPPER_INPUT:
      //emit userInput(foo->data, foo->session);
      qDebug() << "* MMapperPluginParser's thread is"
	       << _parsers[foo->session]->thread();
      _parsers[foo->session]->moveToThread(thread());
      _parsers[foo->session]->userInput(foo->data, foo->session);
      qDebug() << "* MMapperPluginParser's thread is"
	       << _parsers[foo->session]->thread();
      break;
    case MMAPPER_LOAD_MAP:
      //emit loadFile("/home/nschimme/arda.mm2");
      _mappers[foo->session]->loadFile("/home/nschimme/arda.mm2");
      break;
    };

    delete foo;
  }
  qDebug() << "* MMapperPlugin::run() returning. Running in" << thread();
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
    MMapperPluginParser *mpp = new MMapperPluginParser(s, this, mm);

    connect(this, SIGNAL(loadFile(const QString&)),
	    mm, SLOT(loadFile(const QString&)));
    
    _mappers.insert(s, mm);
    _parsers.insert(s, mpp);
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
    foreach(MMapperPluginParser *mpp, _parsers.values(s)) {
        delete mpp;
        qDebug() << "* removed MMapperPluginParser for session" << s;
    }
    _parsers.remove(s);
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
