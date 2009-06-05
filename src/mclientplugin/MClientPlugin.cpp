#include "MClientPlugin.h"

#include "MClientEvent.h"
#include "MClientEventData.h"
#include "PluginManager.h"

#include <QApplication>
#include <QEvent>
#include <QString>
#include <QStringList>
#include <QVariant>


MClientPlugin::MClientPlugin(QObject* parent) : QThread(parent) {
    _shortName = "mclientplugin";
    _longName = "The Original MClientPlugin";
    _description = "If you see this text, the plugin author did not replace the default description.";
    _configurable = false;
    _configVersion = "none";
}


MClientPlugin::~MClientPlugin() {
}


const MClientPluginType& MClientPlugin::type() const {
    return _type;
}


/*
const QString& MClientPlugin::libName() const {
    return _libName;
}
*/


const QString& MClientPlugin::shortName() const {
    return _shortName;
}


const QString& MClientPlugin::longName() const {
    return _longName;
}


const QString& MClientPlugin::description() const {
    return _description;
}


const QString& MClientPlugin::version() const {
    return _version;
}


const QHash<QString, int> MClientPlugin::implemented() const {
    return _implemented;
}


const QHash<QString, int> MClientPlugin::dependencies() const {
    return _dependencies;
}


const QStringList& MClientPlugin::dataTypes() const {
    return _dataTypes;
}


const bool MClientPlugin::configurable() const {
    return _configurable;
}


// This is needed in Qt 4.3 because run is pure virtual -- not in 4.4
void MClientPlugin::run() {
}


// Post an event
void MClientPlugin::postEvent(QVariant* payload, QStringList tags, 
        QString session) {
    PluginManager* pm = PluginManager::instance();
    MClientEvent* me = new MClientEvent(new MClientEventData(payload), tags,
					session);

    QApplication::postEvent(pm, me);
    // Why post it if you can access the object directly!?
    //pm->customEvent(me);
}


void MClientPlugin::stopAllSessions() {
    foreach(QString s, _runningSessions) {
        stopSession(s);
    }
}

/*
void MClientPlugin::configure() {
    // Suppose we have a method like this that we want to make pure virtual in
    // the future, so that all plugins are required to implement it.
    //
    // One way to smooth the transition is to implement it here, but put a
    // qWarning or a Logger message to the effect of:
    //
    // MClientPlugin::configure() will become pure virtual in the future.
    // Plugin developers should implement it before that happens to avoid
    // breakage!

}


const bool MClientPlugin::loadSettings() {
    return true;
}

        
const bool MClientPlugin::saveSettings() const {
    return true;
}


void MClientPlugin::customEvent(QEvent* e) {

}
*/
