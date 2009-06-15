#include "PluginEntry.h"

#include <QHash>
#include <QString>
#include <QStringList>


PluginEntry::PluginEntry() {
}


PluginEntry::~PluginEntry() {
}


const QString& PluginEntry::libName() const {
    return _libName;
}


void PluginEntry::libName(const QString name) {
    _libName = name;
}


const QString& PluginEntry::shortName() const {
    return _shortName;
}


void PluginEntry::shortName(const QString name) {
    _shortName = name;
}


const QString& PluginEntry::longName() const {
    return _longName;
}


void PluginEntry::longName(const QString name) {
    _longName = name;
}


const QStringList PluginEntry::apiList() const {
    return QStringList(_apiVersions.keys());
}


void PluginEntry::addAPI(const QString api, int version) {
    _apiVersions.insert(api, version);
}


int PluginEntry::version(const QString& api) const {
    return _apiVersions.contains(api) ? _apiVersions.value(api) : 0; 
}
