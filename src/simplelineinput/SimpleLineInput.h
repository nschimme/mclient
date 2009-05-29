#ifndef SIMPLELINEINPUT_H
#define SIMPLELINEINPUT_H

#include "MClientIOPlugin.h"

#include <QHash>

class InputWidget;
class QEvent;

class SimpleLineInput : public MClientIOPlugin {
    Q_OBJECT
    
    public:
        SimpleLineInput(QWidget* parent=0);
        ~SimpleLineInput();

        // Plugin members
        void customEvent(QEvent* e);
        void configure();
        const bool loadSettings();
        const bool saveSettings() const;
        const bool startSession(QString s);
        const bool stopSession(QString s);

        // Display members
        const bool initDisplay(QString s);
        const QWidget* getWidget(QString s);

        void sendUserInput(const QString&, const QString&);

    private:
	QString _settingsFile;
	QHash<QString, InputWidget*> _widgets;
};


#endif /* SIMPLELINEINPUT_H */
