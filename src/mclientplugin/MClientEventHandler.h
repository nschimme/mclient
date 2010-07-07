#ifndef MCLIENTEVENTHANDLER_H
#define MCLIENTEVENTHANDLER_H

#include "AbstractPluginSession.h"
#include "MClientDefinitions.h"

#include <QObject>
#include <QMultiHash>
#include <QStringList>
#include <QSet>

class MClientPlugin;

class ConfigEntry;
class SmartMenu;
class MClientEventData;

typedef QSet<SmartMenu*> MenuData;

class MClientEventHandler : public QObject {
    Q_OBJECT
    
    public:
        MClientEventHandler(AbstractPluginSession *ps, MClientPlugin *parent=0);
        virtual ~MClientEventHandler();

        // Handles custom events
        virtual void customEvent(QEvent *e)=0;

	// Generate the menus
	virtual const MenuData& createMenus();

	// The next EventHandler to forward the event to
	void setNextHandler(const QString &s, MClientEventHandler *eh);

	// Forward the event to the next in the event chain
	void forwardEvent(QEvent *e);

public slots:
	// To be removed?
	void postManager(QVariant* payload, QStringList tags, QString session);

        // Post an event to the PluginSession
	void postSession(QVariant* payload, QStringList types);

    protected:
	// Configuration Settings
	ConfigEntry *_config;

	// Menus
	MenuData _menus;

	// For the Event passing to the next EventHandler
	QHash<QString, MClientEventHandler*> _nextEventHandler;

	AbstractPluginSession* _pluginSession;
	QString _session;
};


#endif /* MCLIENTEVENTHANDLER_H */
