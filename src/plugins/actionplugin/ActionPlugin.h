#ifndef ACTIONPLUGIN_H
#define ACTIONPLUGIN_H

#include "MClientPlugin.h"

#include <QHash>
#include <QMultiMap>
#include <QMultiHash>

class QEvent;
class Action;

typedef QString Session;
struct ActionHolder;

class ActionPlugin : public MClientPlugin {
    Q_OBJECT
    
    public:
        ActionPlugin(QObject* parent=0);
        ~ActionPlugin();

        // Plugin members
        void customEvent(QEvent* e);
        void configure();
        const bool loadSettings();
        const bool saveSettings() const;
        const bool startSession(QString s);
        const bool stopSession(QString s);

    private:
	QHash<Session, ActionHolder*> _sessions;

	bool handleCommand(const QString&, const Session&);
	bool parseAction(const QString&, QStringList, const Session&);
        void displayData(const QString&, const QString&);
};

#endif /* ACTIONPLUGIN_H */
