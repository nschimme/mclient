#include "MClientEventHandler.h"

#include "MClientPlugin.h"

#include "MClientEvent.h"
#include "MClientEngineEvent.h"
#include "MClientEventData.h"

#include "ConfigEntry.h"

#include <QApplication>
#include <QEvent>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QDebug>

MClientEventHandler::MClientEventHandler(AbstractPluginSession *ps, MClientPlugin *parent)
  : QObject(parent), _pluginSession(ps) {
  _session = _pluginSession->session();
  _config = ps->retrievePluginSettings(parent->shortName());
}


MClientEventHandler::~MClientEventHandler() {
}


// Post an event
void MClientEventHandler::postSession(QVariant* payload, QStringList tags) {
  MClientEventData *med = new MClientEventData(payload, tags, _session);
  MClientEvent* me = new MClientEvent(med);
  QCoreApplication::postEvent(_pluginSession, me);
}


void MClientEventHandler::postManager(QVariant* /*payload*/, QStringList /*tags*/, QString /*session*/) {
  //MClientEvent* me = new MClientEvent(new MClientEventData(payload, tags, session));
  //QApplication::postEvent(_pluginSession->getManager(), me);
}


const MenuData& MClientEventHandler::createMenus() {
  return _menus;
}


void MClientEventHandler::setNextHandler(const QString &s,
					 MClientEventHandler *eh) {
  _nextEventHandler[s] = eh;
}


void MClientEventHandler::forwardEvent(QEvent *e) {
  if (e->type() == 10001) {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    foreach (QString s, me->dataTypes()) {
      // Need to make a copy, since the original event
      // will be deleted when this function returns
      MClientEvent* nme = new MClientEvent(*me);
      MClientEventHandler *eh = _nextEventHandler[s];
      if (eh) QCoreApplication::postEvent(eh, nme);
    }
  }
}
