#ifndef MCLIENTPLUGININTERFACE_H
#define MCLIENTPLUGININTERFACE_H

#include <QtPlugin>
#include <QHash>

class QString;
enum MClientPluginType { UNKNOWN = 0, FILTER, DISPLAY, IO };

class MClientPluginInterface {
    
    public:
        // The type of plugin
        virtual const MClientPluginType& type() const=0;

        // The library filename relative to plugins dir
//        virtual const QString& libName() const=0;

        // The short name of the plugin used in hashes and maps
        virtual const QString& shortName() const=0;

        // The long name of the plugin displayed in the gui
        virtual const QString& longName() const=0;

        // The description of the plugin shown to the user
        virtual const QString& description() const=0;

        // The plugin version string
        virtual const QString& version() const=0;

        // Returns a QStringList of APIs this plugin implements to be used
        // when checking dependencies.
        virtual const QHash<QString, int> implemented() const=0;
        
        // Returns a QStringList of APIs this plugin requires to be
        // implemented before it can be loaded.
        virtual const QHash<QString, int> dependencies() const=0;
        
        // Returns a QStringList of data types it cares about
        virtual const QStringList& dataTypes() const=0;

        // Consider putting this here and leaving it virtual.
        virtual void customEvent(QEvent* e)=0;

        // Can this be configured manually?
        virtual const bool configurable() const=0;

        // If so, we need to implement this
        virtual void configure()=0;

        // Also, all plugins need to have the ability to load settings.
        virtual const bool loadSettings()=0;

        // And also they need to save them.
        virtual const bool saveSettings() const=0;

        // Create objects local to one session
        virtual const bool startSession(QString s)=0;

        // Destroy objects local to one session
        virtual const bool stopSession(QString s)=0;

        // Post an event to the PluginManager
        virtual void postEvent(QVariant* payload, QStringList types,
                QString session)=0;

};

Q_DECLARE_INTERFACE(MClientPluginInterface,
        "mume.mclient.MClientPluginInterface/1.0")

#endif // MCLIENTPLUGININTERFACE_H
