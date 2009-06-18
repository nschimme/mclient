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

        // Use QSettings to handle general app-level settings
        bool readApplicationSettings();
        bool writeApplicationSettings();
	QHash<QString, QString>* applicationSettings() {
	  return _appSettings;
	}

        // Take care of plugin settings that are stored in xml
	bool readPluginSettings(const QString &pluginName);
	bool writePluginSettings(const QString &pluginName);
	QHash<QString, QString>* pluginSettings(const QString &pluginName) {
	  return _pluginSettings[pluginName];
	}

        // Return a list of profile names
        const QStringList profileNames() const {
	  return _profilePlugins.uniqueKeys();
	}

        // Return a list of plugins for a given profile
        const QStringList profilePlugins(const QString &s) const {
	  return _profilePlugins.value(s);
	}

	// For Profile management
	bool duplicateProfile(const QString &, const QString &);
	bool deleteProfile(const QString &);
	QAbstractTableModel *model () const;


    protected:
        // It's a singleton, so these go here
        ConfigManager(QObject* parent=0);

        static ConfigManager* _pinstance;


    private:
        // Where should xml files be read from?
        QString _configPath;

	// Application's settings
	QHash<QString, QString> *_appSettings;

	// Hash of each plugin's settings
	QHash<QString, QHash<QString, QString>* > _pluginSettings;

	// List of plugins for a given profile
	QHash<QString, QStringList> _profilePlugins;

	ConfigModel *_model;
};


#endif /* CONFIGMANAGER_H */
