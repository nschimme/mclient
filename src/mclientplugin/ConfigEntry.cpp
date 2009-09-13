#include "ConfigEntry.h"
#include "ConfigManager.h"

#include <QHash>

ConfigEntry::ConfigEntry(const QString pluginName) {
  _modified = false;
  _pluginName = pluginName;
  _settings = new SettingsHash;
}


ConfigEntry::~ConfigEntry() {
  delete _settings;
}


const QString& ConfigEntry::pluginName() const {
  return _pluginName;
}


SettingsHash* ConfigEntry::hash() const {
  return _settings;
}


void ConfigEntry::hash(SettingsHash *hash) {
  delete _settings;
  _settings = hash;
}


void ConfigEntry::insert(const QString &key, const QVariant &value) {
  _modified = true;
  _settings->insert(key, value);
}


QVariant ConfigEntry::value(const QString &key,
			    const QVariant &defaultValue) const {
  return _settings->value(key, defaultValue);
}
