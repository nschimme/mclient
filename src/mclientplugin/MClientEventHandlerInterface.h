#ifndef MCLIENTEVENTHANDLERINTERFACE_H
#define MCLIENTEVENTHANDLERINTERFACE_H

#include <QtPlugin>
#include <QEvent>
#include <QVariant>

//#include "MClientPlugin.h"

class SmartMenu;
class MClientPlugin;

typedef QSet<SmartMenu*> MenuData;

class MClientEventHandlerInterface {
    
    public:
        // Handles custom events
        virtual void customEvent(QEvent *e)=0;

	// Generate the menus
	virtual const MenuData& createMenus()=0;

        // Post an event to the PluginSession
	virtual void postSession(QVariant* payload, QStringList types)=0;

        // Post an event to the PluginManager
	virtual void postManager(QVariant* payload, QStringList types, 
				 QString session)=0;

	// Post the parent plugin
	virtual MClientPlugin* plugin()=0;

    protected:
	// Handles MClientEngineEvents
	void engineEvent(QEvent *e);

};

Q_DECLARE_INTERFACE(MClientEventHandlerInterface,
        "mume.mclient.MClientEventHandlerInterface/1.0")

#endif // MCLIENTEVENTHANDLERINTERFACE_H
