#include "MClientPlugin.h"

#include "PluginManager.h"
#include "PluginSession.h"
#include "MClientEvent.h"
#include "MClientEngineEvent.h"
#include "MClientEventData.h"

#include <QApplication>
#include <QEvent>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QDebug>

#include <QPluginLoader>

MClientPlugin::MClientPlugin(QObject* parent) : QThread(parent) {
    _shortName = "mclientplugin";
    _longName = "The Original MClientPlugin";
    _description = "If you see this text, the plugin author did not replace the default description.";
    _configurable = false;
    _configVersion = "none";
}


MClientPlugin::~MClientPlugin() {
}


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


const QStringList& MClientPlugin::receivesDataTypes() const {
    return _receivesDataTypes;
}


const QStringList& MClientPlugin::deliversDataTypes() const {
    return _deliversDataTypes;
}


bool MClientPlugin::configurable() const {
    return _configurable;
}


#if QT_VERSION < 0x0040400
// This is needed in Qt 4.3 because run is pure virtual -- not in 4.4
void MClientPlugin::run() {
}
#endif

void MClientPlugin::setPluginManager(PluginManager *pm) {
  _pluginManager = pm;
}
