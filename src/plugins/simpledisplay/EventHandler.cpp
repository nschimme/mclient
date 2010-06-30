#include "EventHandler.h"

#include <QEvent>
#include <QVariant>

#include "MClientEvent.h"
#include "PluginSession.h"

#include "ClientTextEdit.h"

// Menus
#include "SmartMenu.h"
#include <QAction>

EventHandler::EventHandler(PluginSession *ps, MClientPlugin *mp)
  : MClientDisplayHandler(ps, mp) {
  // Allowable Display Locations
  SET(_displayLocations, DL_DISPLAY);
}


EventHandler::~EventHandler() {
}


void EventHandler::customEvent(QEvent *e) {
  if(!e->type() == 10001) return;

  // Forward the event to the next in the chain
  forwardEvent(e);
  
  MClientEvent* me = static_cast<MClientEvent*>(e);
  if (me->dataTypes().contains("DisplayData") ||
      me->dataTypes().contains("DisplayPrompt") ||
      me->dataTypes().contains("UserInput")) {
    emit displayText(me->payload()->toString());

  }
  else if (me->dataTypes().contains("DoneLoading")) {
    emit displayText(QString("Type \033[1m#help\033[0m for help.\r\n"));

  }
}

QWidget* EventHandler::createWidget() {
  _widget = new ClientTextEdit;

  connect(this, SIGNAL(displayText(const QString&)),
	  _widget, SLOT(displayText(const QString&)));

  return _widget;
}

// Menu
const MenuData& EventHandler::createMenus() {
  QAction *settingsAct = new QAction(tr("Select &Font"), 0);
  settingsAct->setStatusTip(tr("Change the font used in the display"));
  connect(settingsAct, SIGNAL(triggered()), _widget, SLOT(changeFont()) );

  SmartMenu *viewMenu = new SmartMenu(tr("&View"), 10, 2);
  viewMenu->addSeparator();
  viewMenu->addAction(settingsAct);

  _menus.insert(viewMenu);

  return _menus;
}
