#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>

#include <QHash>
#include <QStringList>
#include <QSettings>

bool readXmlFile(QIODevice &device, QSettings::SettingsMap &map);
bool writeXmlFile(QIODevice &device, const QSettings::SettingsMap &map);
bool sortXmlElements(const QString &s1, const QString &s2);

class QAbstractTableModel;
class ConfigModel;

class ConfigManager : public QObject {
    Q_OBJECT

    public:
        // Singleton methods
        static ConfigManager* instance();
        ~ConfigManager();

        // General app-level settings
        bool readApplicationSettings();
        bool writeApplicationSettings();
	QHash<QString, QString>* applicationSettings() {
	  return _appSettings;
	}

	// Profile settings
	bool readProfileSettings(const QString &dirName);
	bool writeProfileSettings(const QString &profileName);
	QHash<QString, QString>* profileSettings(const QString &profileName) {
	  return _profileSettings[profileName];
	}

        // Take care of plugin settings that are stored in xml
	bool readPluginSettings(const QString&, const QString&);
	bool writePluginSettings(const QString&, const QString&);
	QHash<QString, QString>* pluginSettings(const QString &profileName,
						const QString &pluginName) {
	  return _pluginSettings[profileName]->value(pluginName);
	}

	// Discover which profiles are valid
	bool discoverProfiles();

        // Return a list of profile names
        const QStringList profileNames() const {
	  return _profileSettings.uniqueKeys();
	}

        // Return a list of plugins for a given profile
        const QStringList profilePlugins(const QString &s) const {
	  return _pluginSettings.value(s)->keys();
	}

	// For Profile management
	bool duplicateProfile(const QString &, const QString &);
	bool deleteProfile(const QString &);
	QAbstractTableModel *model () const;

	// Where are profiles located?
	QString getPluginPath() { return _pluginPath; }


    protected:
        // It's a singleton, so these go here
        ConfigManager(QObject* parent=0);

        static ConfigManager* _pinstance;


    private:
        // Where should xml files be read from?
        QString _configPath;

        // In what directory are plugins located?
        QString _pluginPath;

	// Application's settings
	QHash<QString, QString> *_appSettings;

	// Profile's settings
	QHash<QString, // profile
	  QHash<QString, QString>* // settings
	  > _profileSettings;
	
	// Hash of each profile's plugin settings
	QHash<QString, // profile
	  QHash<QString, // plugin
	    QHash<QString, QString>* // settings
	  >*
	> _pluginSettings;

	ConfigModel *_model;
};


#endif /* CONFIGMANAGER_H */
