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

MClientPlugin::MClientPlugin(QObject* parent) : QObject(parent) {
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


const QHash<QString, int> MClientPlugin::receivesDataTypes() const {
    return _receivesDataTypes;
}


bool MClientPlugin::configurable() const {
    return _configurable;
}


const QList<CommandEntry* > MClientPlugin::commandEntries() const {
  return _commandEntries;
}
