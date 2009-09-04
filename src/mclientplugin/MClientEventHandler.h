#ifndef MCLIENTEVENTHANDLER_H
#define MCLIENTEVENTHANDLER_H

#include <QObject>
#include <QMultiHash>
#include <QStringList>
#include <QSet>

class PluginSession;
class SmartMenu;
class MClientEventData;

typedef QSet<SmartMenu*> MenuData;

class MClientEventHandler : public QObject {
    Q_OBJECT
    
    public:
        MClientEventHandler(QObject* parent=0);
        ~MClientEventHandler();

        // Handles custom events
        virtual void customEvent(QEvent *e)=0;

	// Receive the PluginSession reference upon load
	void setPluginSession(PluginSession *ps);

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

	// References
	PluginSession *_pluginSession;

	// Menus
	MenuData _menus;
};


#endif /* MCLIENTEVENTHANDLER_H */
