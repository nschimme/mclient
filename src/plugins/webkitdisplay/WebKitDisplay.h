#ifndef WEBKITDISPLAY_H
#define WEBKITDISPLAY_H

#include "MClientDisplayPlugin.h"

#include <QHash>
#include <QPointer>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>

class DisplayWidget;
class QEvent;

class WebKitDisplay : public MClientDisplayPlugin {
    Q_OBJECT
    
    public:
        WebKitDisplay(QWidget* parent=0);
        ~WebKitDisplay();

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

	static const QChar greaterThanChar;
	static const QChar lessThanChar;
	static const QString greaterThanTemplate;
	static const QString lessThanTemplate;

    private:
	bool _foreground, _background, _bold, _underline;
	bool _blink, _inverse, _strikethrough;

	QPointer<DisplayWidget> _widget;

	void parseDisplayData(QString);
	QString convertANSI(int code);

	bool _quit;
	QMutex _mutex;
	QWaitCondition _cond;
	QQueue< QString > _eventQueue;

 signals:
	void dataReceived(const QString&);
};


#endif /* WEBKITDISPLAY_H */
