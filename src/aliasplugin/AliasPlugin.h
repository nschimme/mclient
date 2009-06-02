#ifndef ALIASPLUGIN_H
#define ALIASPLUGIN_H

#include "MClientPlugin.h"

#include <QHash>

class QEvent;
class Alias;

typedef QString Session;
struct AliasHolder;

class AliasPlugin : public MClientPlugin {
    Q_OBJECT
    
    public:
        AliasPlugin(QObject* parent=0);
        ~AliasPlugin();

        // Plugin members
        void customEvent(QEvent* e);
        void configure();
        const bool loadSettings();
        const bool saveSettings() const;
        const bool startSession(QString s);
        const bool stopSession(QString s);

    private:
	QHash<Session, AliasHolder*> _sessions;

	bool handleCommand(const QString&, const Session&);
	void parseInput(const QString&, const Session&);
        void displayData(const QString&, const QString&);
};

#endif /* ALIASPLUGIN_H */
