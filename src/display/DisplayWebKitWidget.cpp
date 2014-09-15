#include "DisplayWebKitWidget.h"
#include "DisplayParser.h"

#include <QDebug>
#include <QtWebKitWidgets>
#include <QWebFrame>
#include <QWebElement>

#include <QTimer> // Debug (for identifying lag)

//#include <QFile>

DisplayWebKitWidget::DisplayWebKitWidget(QWidget* parent) : QWebView(parent) {
    QWidget::setMinimumSize(0, 30);
    _ansiParser = new DisplayParser(this);

    // Sections Information
    _currentSection = 0;
    _maxSections = 1;
    _currentCharacterCount = 0;
    _maxCharacterCount = 300000000;
    _scrollToBottom = true;

    // Create WebKit Display
    page()->mainFrame()->load(QUrl("qrc:/webkitdisplay/page.html"));

    // Connect Signals/Slots
    connect(this, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));
    connect(page(), SIGNAL(contentsChanged()), SLOT(scrollToBottom()));

    // TODO: Why aren't frames working?
    connect(page(), SIGNAL(frameCreated(QWebFrame *frame)),
            SLOT(loadFrame(QWebFrame *frame)));

    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    page()->setContentEditable(false);
}


DisplayWebKitWidget::~DisplayWebKitWidget() {
}


void DisplayWebKitWidget::appendText(const QString &output) {
    QString html = _ansiParser->parseDisplayData(output);

    _scrollToBottom = true;

    // Maximum Amount of Permitted Characters/Sections
    _currentCharacterCount += html.size();
    if (_currentCharacterCount >= _maxCharacterCount) {
        _currentCharacterCount = 0;

        if (_currentSection >= _maxSections)
            _currentSection = 0;
        else
            _currentSection++;

        // Replace/Blank Section
    } else {
        // Append text to Section
        QTime t;
        t.start();
        QWebElement doc = page()->mainFrame()->documentElement();
        doc.findFirst(".section").appendInside(html);
        //qDebug() << "* Displaying (" << t.elapsed() << "ms):" << output;
    }


    //scrollToBottom();
}

void DisplayWebKitWidget::finishLoading(bool) {
    qDebug() << "* WebKit page loaded";
}

void DisplayWebKitWidget::paintEvent(QPaintEvent *ev) {
    // When the display is updated we want to (usually) always scroll to the bottom
    // TODO: fix this so when the user scrolls it doesn't scroll to the bottom
    QWebView::paintEvent(ev);
    if (_scrollToBottom) scrollToBottom();
}

void DisplayWebKitWidget::scrollToBottom() {
    int height = page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
    page()->mainFrame()->setScrollBarValue(Qt::Vertical, height);
    _scrollToBottom = false;
}


void DisplayWebKitWidget::loadFrame(QWebFrame *frame) {
    qDebug() << "frame created!" << frame->frameName();
}
