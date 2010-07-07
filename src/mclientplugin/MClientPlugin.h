#ifndef MCLIENTPLUGIN_H
#define MCLIENTPLUGIN_H

#include "MClientPluginInterface.h"

#include <QObject>
#include <QtPlugin>
#include <QHash>
#include <QPointer>

class MClientEventData;
class MClientEventHandler;

class MClientPlugin : public QObject, public MClientPluginInterface {
    Q_OBJECT
    Q_INTERFACES(MClientPluginInterface)
    
    public:
        MClientPlugin(QObject* parent=0);
        virtual ~MClientPlugin();

        // The short name of the plugin used in hashes and maps
        const QString& shortName() const;

        // The long name of the plugin displayed in the gui
        const QString& longName() const;

        // The description of the plugin shown to the user
        const QString& description() const;

        // The plugin version string
        const QString& version() const;

        // Returns a QStringList of APIs this plugin implements to be used
        // when checking dependencies.
        const QHash<QString, int> implemented() const;
        
        // Returns a QHash of APIs and versions this plugin requires to be
        // implemented before it can be loaded.
        const QHash<QString, int> dependencies() const;

        // Returns a QHash of data types and priorities
        const QHash<QString, int> receivesDataTypes() const;

        // Can this be configured manually?
        bool configurable() const;

        // If so, we need to implement this
        virtual void configure()=0;

        // Create objects local to one session
        virtual bool startSession(AbstractPluginSession *ps)=0;

        // Destroy objects local to one session
        virtual bool stopSession(const QString &session)=0;

	// Grab the event handler
        virtual MClientEventHandler* getEventHandler(const QString &s)=0;
        
	// List of commands
	const QList<CommandEntry* > commandEntries() const;

    protected:
        QString _shortName, _longName, _description, _version;

        bool _configurable;
        QString _configVersion;

        QHash<QString, int> _implemented, _dependencies;
        QHash<QString, int> _receivesDataTypes;

	QList<CommandEntry* > _commandEntries;
};


#endif /* MCLIENTPLUGIN_H */
