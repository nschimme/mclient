#ifndef MCLIENTEVENTHANDLER_H
#define MCLIENTEVENTHANDLER_H

#include <QObject>
#include <QMultiHash>
#include <QStringList>
#include <QSet>

class MClientPlugin;
class PluginSession;

class ConfigEntry;
class SmartMenu;
class MClientEventData;

typedef QSet<SmartMenu*> MenuData;

class MClientEventHandler : public QObject {
    Q_OBJECT
    
    public:
        MClientEventHandler(PluginSession *ps, MClientPlugin *pl);
        ~MClientEventHandler();

        // Handles custom events
        virtual void customEvent(QEvent *e)=0;

	// Generate the menus
	virtual const MenuData& createMenus();

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
};


#endif /* MCLIENTEVENTHANDLER_H */
