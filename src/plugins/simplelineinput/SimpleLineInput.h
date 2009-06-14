#ifndef SIMPLELINEINPUT_H
#define SIMPLELINEINPUT_H

#include "MClientDisplayPlugin.h"

#include <QHash>
#include <QPointer>

class InputWidget;
class QEvent;

class SimpleLineInput : public MClientDisplayPlugin {
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
        QWidget* getWidget(QString s);

public slots:
        void sendUserInput(const QString&);

    private:
	QString _settingsFile;
	QPointer<InputWidget> _widget;
};


#endif /* SIMPLELINEINPUT_H */
