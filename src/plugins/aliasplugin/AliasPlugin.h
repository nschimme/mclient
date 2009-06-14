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
	AliasHolder *h;

	bool handleCommand(const QString&);
	void parseInput(const QString&);
        void displayData(const QString&);
};

#endif /* ALIASPLUGIN_H */
