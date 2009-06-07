#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>

#include <QHash>
#include <QStringList>
#include <QSettings>

bool readXmlFile(QIODevice &device, QSettings::SettingsMap &map);
bool writeXmlFile(QIODevice &device, const QSettings::SettingsMap &map);
bool sortXmlElements(const QString &s1, const QString &s2);

class ConfigManager : public QObject {
    Q_OBJECT

    public:
        // Singleton methods
        static ConfigManager* instance();
        void destroy();

        // Use QSettings to handle general app-level settings
        const bool readApplicationSettings();
        const bool writeApplicationSettings();
	QHash<QString, QString>* applicationSettings() {
	  return _appSettings;
	}

        // Take care of plugin settings that are stored in xml
	const bool readPluginSettings(const QString &pluginName);
	const bool writePluginSettings(const QString &pluginName);
	QHash<QString, QString>* pluginSettings(const QString &pluginName) {
	  return _pluginSettings[pluginName];
	}

        // Return a list of profile names
        const QStringList profileNames() const {
	  return _profiles.uniqueKeys();
	}

        // Return a list of plugins for a given profile
        const QStringList profilePlugins(const QString profile) const;


    protected:
        // It's a singleton, so these go here
        ConfigManager(QObject* parent=0);
        ~ConfigManager();

        static ConfigManager* _pinstance;


    private:
        // Where should xml files be read from?
        QString _configPath;

	// Application's settings
	QHash<QString, QString> *_appSettings;

	// Hash of each plugin's settings
	QHash<QString, QHash<QString, QString>* > _pluginSettings;

	// List of plugins for a given profile
	QMultiHash<QString, QString> _profiles;

};


#endif /* CONFIGMANAGER_H */
