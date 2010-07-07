#ifndef MCLIENTPLUGININTERFACE_H
#define MCLIENTPLUGININTERFACE_H

#include <QtPlugin>
#include <QHash>
#include <QPointer>

class AbstractPluginSession;
class MClientEventHandler;
class CommandEntry;
class ConfigEntry;

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
        
        // Returns a QHash of data types and priorities it needs
        virtual const QHash<QString, int> receivesDataTypes() const=0;

        // Can this be configured manually?
        virtual bool configurable() const=0;

        // If so, we need to implement this
        virtual void configure()=0;

        // Create objects local to one session
        virtual bool startSession(AbstractPluginSession *ps)=0;

        // Destroy objects local to one session
        virtual bool stopSession(const QString &session)=0;

	// Grab the event handler
	virtual MClientEventHandler* getEventHandler(const QString &session)=0;

	// List of commands
	virtual const QList<CommandEntry* > commandEntries() const=0;

};

Q_DECLARE_INTERFACE(MClientPluginInterface,
        "mume.mclient.MClientPluginInterface/1.0")

#endif // MCLIENTPLUGININTERFACE_H
