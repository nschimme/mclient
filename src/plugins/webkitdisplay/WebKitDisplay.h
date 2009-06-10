#ifndef WEBKITDISPLAY_H
#define WEBKITDISPLAY_H

#include "MClientDisplayPlugin.h"

#include <QHash>

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
        const bool loadSettings();
        const bool saveSettings() const;
        const bool startSession(QString s);
        const bool stopSession(QString s);

        // Display members
        const bool initDisplay(QString s);
        QWidget* getWidget(QString s);

  protected:
	static const QChar greaterThanChar;
	static const QChar lessThanChar;
	static const QString greaterThanTemplate;
	static const QString lessThanTemplate;

    private:
	bool _foreground, _background, _bold, _underline;
	bool _blink, _inverse, _strikethrough;

	QHash<QString, DisplayWidget*> _widgets;

	void parseDisplayData(QString, const QString&);
	QString convertANSI(int code);

 signals:
	void dataReceived(const QString&, const QString&);
};


#endif /* WEBKITDISPLAY_H */
