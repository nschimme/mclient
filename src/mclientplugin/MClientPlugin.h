#ifndef MCLIENTPLUGIN_H
#define MCLIENTPLUGIN_H

#include "MClientPluginInterface.h"

#include <QThread>
#include <QtPlugin>
#include <QHash>
#include <QStringList>

class PluginSession;
class MClientEventData;

class MClientPlugin : public QThread, public MClientPluginInterface {
    Q_OBJECT
    Q_INTERFACES(MClientPluginInterface)
    
    public:
        MClientPlugin(QObject* parent=0);
        ~MClientPlugin();

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

        // Returns a QStringList of data types it cares about
        const QStringList& receivesDataTypes() const;

        // Returns a QStringList of data types it delivers
        const QStringList& deliversDataTypes() const;
        
        // Handles custom events
        virtual void customEvent(QEvent *e)=0;

	// Handles MClientEngineEvents
	void engineEvent(QEvent *e);
        
        // Can this be configured manually?
        bool configurable() const;

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

#if QT_VERSION < 0x0040400
        // Needed for Qt 4.3, but not for 4.4
        virtual void run();
#endif

	// Receive the PluginSession reference upon load
	void setPluginSession(PluginSession *ps);

public slots:
        // Post an event to the PluginSession
	void postSession(QVariant* payload, QStringList types);

        // Post an event to the PluginManager
	void postManager(QVariant* payload, QStringList types, 
			 QString session);

    protected:
        QString _shortName, _longName, _description, _version;

        bool _configurable;
        QString _configVersion;

        QHash<QString, int> _implemented, _dependencies;

        QStringList _receivesDataTypes, _deliversDataTypes;
	QMultiHash<QString, QObject*> _receivesTypes;

	// References
	PluginSession *_pluginSession;

	// Is this necessary?
	QString _session;

};


#endif /* MCLIENTPLUGIN_H */
