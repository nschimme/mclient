#ifndef SIMPLEDISPLAY_H
#define SIMPLEDISPLAY_H

#include "MClientDisplayPlugin.h"

#include <QPointer>

class ClientTextEdit;
class QEvent;

class SimpleDisplay : public MClientDisplayPlugin {
    Q_OBJECT
    
    public:
        SimpleDisplay(QWidget* parent=0);
        ~SimpleDisplay();

        // Plugin members
        void customEvent(QEvent* e);
        void configure();
        bool loadSettings();
        bool saveSettings() const;
        bool startSession(QString s);
        bool stopSession(QString s);

        // Display members
        bool initDisplay(QString s);
        QWidget* getWidget(QString s);

  protected:
	void run();

    private:
	QPointer<ClientTextEdit> _widget;

 signals:
	void displayText(const QString&);
	void userInput(const QString&);
};


#endif /* SIMPLEDISPLAY_H */
