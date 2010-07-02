#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QWidget>
#include <QWebView>

class QWebFrame;
class WebKitDisplay;

// for Qt 4.5, we use JQuery
// for Qt >= 4.6, we use the QWebElement API
#if QT_VERSION < 0x040600
#define USE_JQUERY
#endif

#if QT_VERSION < 0x0040500
#error You need Qt 4.5 or newer
#endif

class DisplayWidget : public QWebView {
    Q_OBJECT
    
    public:
        DisplayWidget(QWidget* parent=0);
        ~DisplayWidget();

public slots:
	void appendText(const QString&);
        void scrollToBottom();
	void loadFrame(QWebFrame *frame);

protected slots:
        void finishLoading(bool);

 protected:
	QWebView* _view;
	void paintEvent(QPaintEvent *ev);

    private:
	int _currentSection, _maxSections;
       	int _currentCharacterCount, _maxCharacterCount;

	bool _scrollToBottom;
};


#endif /* DISPLAYWIDGET_H */
