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
	ActionHolder *h;

	bool handleCommand(const QString&);
	bool parseAction(const QString&, QStringList);
        void displayData(const QString&);
};

#endif /* ACTIONPLUGIN_H */
