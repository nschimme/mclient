#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>

#include <QHash>
#include <QStringList>


class ConfigManager : public QObject {
    Q_OBJECT

    public:
        // Singleton methods
        static ConfigManager* instance();
        void destroy();

        // Use QSettings to handle general app-level settings
        const bool readApplicationSettings();
        const bool writeApplicationSettings() const;

        // Take care of plugin settings that are stored in xml
        const bool readPluginSettings();
        const bool writePluginSettings() const;

        // Return a list of profile names (_profiles.uniqueKeys())
        const QStringList& profileNames() const;

        // Return a list of plugins for a given profile
        const QStringList profilePlugins(const QString profile) const;

        const QHash<QString, QHash<QString, QString> >& 
            pluginProfileConfig(const QString pl, const QString pr) const;


    protected:
        // It's a singleton, so these go here
        ConfigManager(QObject* parent=0);
        ~ConfigManager();

        static ConfigManager* _pinstance;


    private:
        // Where should xml files be read from?
        QString _configPath;

        // Maps profile -> list of plugins in it
        QHash<QString, QStringList> _profiles;
        
        // The config structure in memory
        QHash<QString, // plugin
            QHash<QString, // profile
                QHash<QString, // object id (connection, etc.)
                    QHash<QString, QString> // key->value pairs:
                >
            >
        > _config;

        // Maps plugin shortname -> settings file name 
        QHash<QString, QString> _configFiles;


};


#endif /* CONFIGMANAGER_H */
