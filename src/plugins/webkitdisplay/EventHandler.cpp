#include "EventHandler.h"
#include "DisplayWidget.h"
#include "DisplayParser.h"

#include <QEvent>
#include <QVariant>

#include "MClientEvent.h"
#include "AbstractPluginSession.h"

#include <QtWebKit>

EventHandler::EventHandler(AbstractPluginSession *ps, MClientPlugin *mp)
  : MClientDisplayHandler(ps, mp) {
  // Allowable Display Locations
  SET(_displayLocations, DL_DISPLAY);

    // Set up the Browser
    QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, false);

    // Debugging
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

  _parser = new DisplayParser;

  connect(this, SIGNAL(displayData(const QString&)),
	  _parser, SLOT(displayData(const QString&)));
  connect(this, SIGNAL(userInput(const QString&)),
	  _parser, SLOT(userInput(const QString&)));
}


EventHandler::~EventHandler() {
  _widget->disconnect();
  _widget->deleteLater();
  _parser->deleteLater();
}


void EventHandler::customEvent(QEvent *e) {
  // Forward the event to the next in the chain
  forwardEvent(e);
  
  MClientEvent* me = static_cast<MClientEvent*>(e);
  if (me->dataTypes().contains("DisplayData") ||
      me->dataTypes().contains("DisplayPrompt")) {
    emit displayData(me->payload()->toString());

  }
  else if (me->dataTypes().contains("UserInput")) {
    emit userInput(me->payload()->toString());

  }
  else if (me->dataTypes().contains("DoneLoading")) {
    emit displayData(QString("Type \033[1m#help\033[0m for help.\r\n"));

  }
}

QWidget* EventHandler::createWidget(QWidget *parent) {
  _widget = new DisplayWidget(parent);

  /* I made this blocking so that it doesn't look like the program
     has locked up. The problem is that the Javascript DOM is slow. */
  connect(_parser, SIGNAL(displayText(const QString&)),
	  _widget, SLOT(appendText(const QString&)));
  //,  Qt::BlockingQueuedConnection);

  return _widget;
}
