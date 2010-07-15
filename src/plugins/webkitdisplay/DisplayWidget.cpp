#include "WebKitDisplay.h"
#include "DisplayWidget.h"

#include <QDebug>
#include <QtWebKit>
#include <QWebFrame>
#include <QWebElement>

#include <QTimer> // Debug (for identifying lag)

//#include <QFile>

DisplayWidget::DisplayWidget(QWidget* parent) : QWebView(parent) {
    QWidget::setMinimumSize(0, 30);

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

    /*
#ifdef USE_JQUERY
    QFile file;
    file.setFileName(":/webkitdisplay/jquery.min.js");
    file.open(QIODevice::ReadOnly);
    _jQuery = file.readAll();
    file.close();
#endif
    */
}


DisplayWidget::~DisplayWidget() {
}


void DisplayWidget::appendText(const QString &output) {
    _scrollToBottom = true;

  // Maximum Amount of Permitted Characters/Sections
  _currentCharacterCount += output.size();
  if (_currentCharacterCount >= _maxCharacterCount) {
    _currentCharacterCount = 0;

    if (_currentSection >= _maxSections)
      _currentSection = 0;
    else
      _currentSection++;

    // Replace/Blank Section
#ifdef USE_JQUERY
    QString code = QString("$('.section:first').appendTo('.container').html('%1').show();").arg(output);
    qDebug() << "* Blanking Section:" << code;

#endif
  } else {
      // Append text to Section
    QTime t;
    t.start();
#ifdef USE_JQUERY
    QString code = QString("$('.section').append('%1');").arg(output);
    page()->mainFrame()->evaluateJavaScript(code);
#else
    QWebElement doc = page()->mainFrame()->documentElement();
    doc.findFirst(".section").appendInside(output);
#endif
    //qDebug() << "* Displaying (" << t.elapsed() << "ms):" << output;
  }
  

  //scrollToBottom();
}

void DisplayWidget::finishLoading(bool) {
#ifdef USE_JQUERY
  // Populate Page with Sections
  //QString code = QString("$('.section:first').each( function () { for (var id=1;id<%1;id++) { $('.section:first').clone(true).insertAfter(this); } $('.section').hide(); $('.section:last').show(); } )").arg(_maxSections);
  //page()->mainFrame()->evaluateJavaScript(code);
#endif
  
  qDebug() << "* WebKit page loaded";
}

void DisplayWidget::paintEvent(QPaintEvent *ev) {
  // When the display is updated we want to (usually) always scroll to the bottom
  // TODO: fix this so when the user scrolls it doesn't scroll to the bottom
  QWebView::paintEvent(ev);
  if (_scrollToBottom) scrollToBottom();
}

void DisplayWidget::scrollToBottom() {
#ifdef USE_JQUERY
  QString scrollCode = QString("$(window).scrollTop($(document).height());");
  page()->mainFrame()->evaluateJavaScript(scrollCode);
#else
  int height = page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
  page()->mainFrame()->setScrollBarValue(Qt::Vertical, height);
#endif
  _scrollToBottom = false;
}


void DisplayWidget::loadFrame(QWebFrame *frame) {
  qDebug() << "frame created!" << frame->frameName();
}
