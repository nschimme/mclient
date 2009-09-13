#ifndef CONFIGENTRY_H
#define CONFIGENTRY_H

#include <QHash>
#include <QVariant>

#include "ConfigManager.h"

class ConfigManager;

class ConfigEntry {

    public:
        ConfigEntry(const QString pluginName);
        ~ConfigEntry();

        // The plugin pluginname relative to Configs dir
        const QString& pluginName() const;

	// For hash-like modifications
	void insert(const QString &key, const QVariant &value);

	QVariant value(const QString &key, const QVariant &defaultValue) const;

	SettingsHash* hash() const;
	void hash(SettingsHash *);

	bool isModified() const { return _modified; };

    protected:
        QString _pluginName;
	bool _modified;

        SettingsHash *_settings;

	// Allow direct access by the ConfigManager
	friend class ConfigManager;
};


#endif // CONFIGENTRY_H
