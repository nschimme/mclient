#include "MClientEventHandler.h"

#include "PluginManager.h"
#include "PluginSession.h"

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

MClientEventHandler::MClientEventHandler(PluginSession *ps, MClientPlugin *mp)
  : QObject(mp), _pluginSession(ps), _plugin(mp) {

  // Retrieve the ConfigEntry
  _config = _pluginSession->retrievePluginSettings(_plugin->shortName());
}


MClientEventHandler::~MClientEventHandler() {
}


// Post an event
void MClientEventHandler::postSession(QVariant* payload, QStringList tags) {
  MClientEventData *med = new MClientEventData(payload, tags,
					       _pluginSession->session());
  MClientEvent* me = new MClientEvent(med);
  QCoreApplication::postEvent(_pluginSession, me);
}


void MClientEventHandler::postManager(QVariant* payload, QStringList tags, 
			      QString session) {
  MClientEvent* me = new MClientEvent(new MClientEventData(payload, tags,
							   session));
  
  QApplication::postEvent(_pluginSession->getManager(), me);
}


// Handles MClientEngineEvent
void MClientEventHandler::engineEvent(QEvent *e) {
  MClientEngineEvent* ee = static_cast<MClientEngineEvent*>(e);
  //qDebug() << "#" << _shortName << "got engineEvent" << ee->dataType();
  if (ee->dataType() == EE_MANAGER_POST) {
    // We should never get one of these
    
  }
}


const MenuData& MClientEventHandler::createMenus() {
  return _menus;
}


/*
bool MClientEventHandler::loadSettings(const QHash<QString, QVariant> &hash) {
  _settings = hash;
}
*/

