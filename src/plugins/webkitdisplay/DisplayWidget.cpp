#include "WebKitDisplay.h"
#include "DisplayWidget.h"

#include <QDebug>
#include <QtWebKit>

//#include <QFile>

DisplayWidget::DisplayWidget(QString s, WebKitDisplay* wkd, QWidget* parent) 
    : QWebView(parent) {
    _session = s;
    _wkd = wkd;

    // Sections Information
    _currentSection = 0;
    _maxSections = 1;
    _currentCharacterCount = 0;
    _maxCharacterCount = 300000000;

    // Create WebKit Display
    load(QUrl("qrc:/webkitdisplay/page.html"));

    // Connect Signals/Slots
    connect(this, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));
    connect(_wkd, SIGNAL(dataReceived(const QString&)),
	    SLOT(appendText(const QString&)));

    // Debugging Information
    qDebug() << "* WebKitDisplay thread:" << _wkd->thread();
    qDebug() << "* DisplayWidget thread:" << this->thread();
    
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
    //qDebug() << "* Blanking Section:" << code;

#endif
  } else {
  
    // Append text to Section
#ifdef USE_JQUERY
    //QString code = QString("$('<div
    //class=\"section\">Something!</div>').appendTo('.container');");
    //QString code = QString("$('.section').css('background-color', 'yellow');");
    
    QString code = QString("$('.section:last').append('%1');").arg(output);
    //qDebug() << "* Appending Section:" << code;
    page()->mainFrame()->evaluateJavaScript(code);
  }

  //QString scrollCode = QString("$(window).scrollTop($(document).height());");
  //page()->mainFrame()->evaluateJavaScript(scrollCode);
  int height = page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
  page()->mainFrame()->setScrollBarValue(Qt::Vertical, height);
  
#endif
}

void DisplayWidget::finishLoading(bool) {
#ifdef USE_JQUERY
  // Populate Page with Sections
  QString code = QString("$('.section:first').each( function () { for (var id=1;id<%1;id++) { $('.section:first').clone(true).insertAfter(this); } $('.section').hide(); $('.section:last').show(); } )").arg(_maxSections);
  //page()->mainFrame()->evaluateJavaScript(code);
#endif

  qDebug() << "* WebKit page loaded";
}
