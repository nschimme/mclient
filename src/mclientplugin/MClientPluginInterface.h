#ifndef MCLIENTPLUGININTERFACE_H
#define MCLIENTPLUGININTERFACE_H

#include <QtPlugin>
#include <QHash>

class PluginSession;

class MClientPluginInterface {
    
    public:
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
        virtual const QStringList& receivesDataTypes() const=0;

        // Returns a QStringList of data types it delivers
        virtual const QStringList& deliversDataTypes() const=0;

        // Consider putting this here and leaving it virtual.
        virtual void customEvent(QEvent* e)=0;

        // Can this be configured manually?
        virtual bool configurable() const=0;

        // If so, we need to implement this
        virtual void configure()=0;

        // Also, all plugins need to have the ability to load settings.
        virtual bool loadSettings()=0;

        // And also they need to save them.
        virtual bool saveSettings() const=0;

        // Create objects local to one session
        virtual bool startSession(QString s)=0;

        // Destroy objects local to one session
        virtual bool stopSession(QString s)=0;

	// Receive the PluginSession reference upon load
	virtual void setPluginSession(PluginSession *ps)=0;

};

Q_DECLARE_INTERFACE(MClientPluginInterface,
        "mume.mclient.MClientPluginInterface/1.0")

#endif // MCLIENTPLUGININTERFACE_H
