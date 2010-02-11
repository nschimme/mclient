#ifndef MCLIENTEVENTHANDLER_H
#define MCLIENTEVENTHANDLER_H

#include "MClientEventHandlerInterface.h"
#include "MClientDefinitions.h"

#include <QObject>
#include <QMultiHash>
#include <QStringList>
#include <QSet>

class MClientPlugin;
class PluginSession;

class ConfigEntry;
class SmartMenu;
class MClientEventData;

class MClientEventHandler : public QObject, public MClientEventHandlerInterface {
    Q_OBJECT
    Q_INTERFACES(MClientEventHandlerInterface)
    
    public:
        MClientEventHandler(PluginSession *ps, MClientPlugin *pl);
        virtual ~MClientEventHandler();

        // Handles custom events
        virtual void customEvent(QEvent *e)=0;

	// Generate the menus
	virtual const MenuData& createMenus();

	// Post the parent plugin
	virtual MClientPlugin* plugin();

	// The next EventHandler to forward the event to
	void setNextHandler(const QString &s, MClientEventHandler *eh);

	void forwardEvent(QEvent *e);

public slots:
        // Post an event to the PluginSession
	void postSession(QVariant* payload, QStringList types);

        // Post an event to the PluginManager
	void postManager(QVariant* payload, QStringList types, 
			 QString session);

    protected:
	// Handles MClientEngineEvents
	void engineEvent(QEvent *e);

	// Configuration Settings
	ConfigEntry *_config;

	// Plugin and Session References
	PluginSession *_pluginSession;
	MClientPlugin *_plugin;

	// Menus
	MenuData _menus;

	// For the Event passing to the next EventHandler
	QHash<QString, MClientEventHandler*> _nextEventHandler;
};


#endif /* MCLIENTEVENTHANDLER_H */
