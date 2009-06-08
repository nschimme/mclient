#include "MMapperPlugin.h"

#include "MMapperPluginParser.h"
#include "MapperManager.h"

#include "MClientEvent.h"
#include "PluginManager.h"
#include "CommandManager.h"
#include "ConfigManager.h"
#include "MainWindow.h" // passed into MapperManager

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
    _dataTypes << "SendToSocketData"
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
	     << "map" << "MMapperLoadMap";
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

    if(me->dataTypes().contains("SendToSocketData")) {
	emit userInput(me->payload()->toString(), me->session());

    } else if (me->dataTypes().contains("XMLNone")) {
        emit mudOutput(me->payload()->toString(), me->session());

    } else if (me->dataTypes().contains("XMLName")) {
	emit name(me->payload()->toString(), me->session());

    } else if (me->dataTypes().contains("XMLDescription")) {
	emit description(me->payload()->toString(), me->session());

    } else if (me->dataTypes().contains("XMLDynamicDescription")) {
	emit dynamicDescription(me->payload()->toString(), me->session());
      
    } else if (me->dataTypes().contains("XMLExits")) {
	emit exits(me->payload()->toString(), me->session());

    } else if (me->dataTypes().contains("XMLPrompt")) {
	emit prompt(me->payload()->toString(), me->session());

    } else if (me->dataTypes().contains("XMLMove")) {
	emit move(me->payload()->toString(), me->session());

    } else if (me->dataTypes().contains("MMapperLoadMap")) {
      emit loadFile("/home/nschimme/arda.mm2");

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
    MapperManager *mm = new MapperManager(s, MainWindow::instance());
    MMapperPluginParser *mpp = new MMapperPluginParser(s, mm);
    _mappers.insert(s, mm);
    _parsers.insert(s, mpp);

    connect(this, SIGNAL(name(const QString&, const QString&)),
	    mpp, SLOT(name(const QString&, const QString&)));
    connect(this, SIGNAL(description(const QString&, const QString&)),
	    mpp, SLOT(description(const QString&, const QString&)));
    connect(this, SIGNAL(dynamicDescription(const QString&, const QString&)),
	    mpp, SLOT(dynamicDescription(const QString&, const QString&)));
    connect(this, SIGNAL(exits(const QString&, const QString&)),
	    mpp, SLOT(exits(const QString&, const QString&)));
    connect(this, SIGNAL(prompt(const QString&, const QString&)),
	    mpp, SLOT(prompt(const QString&, const QString&)));
    connect(this, SIGNAL(move(const QString&, const QString&)),
	    mpp, SLOT(move(const QString&, const QString&)));

    connect(this, SIGNAL(userInput(const QString&, const QString&)),
	    mpp, SLOT(userInput(const QString&, const QString&)));
    connect(this, SIGNAL(mudOutput(const QString&, const QString&)),
	    mpp, SLOT(mudOutput(const QString&, const QString&)));

    connect(this, SIGNAL(loadFile(const QString&)),
	    mm, SLOT(loadFile(const QString&)));
    
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
