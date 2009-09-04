#include "MClientEventHandler.h"

#include "PluginManager.h"
#include "PluginSession.h"
#include "MClientEvent.h"
#include "MClientEngineEvent.h"
#include "MClientEventData.h"

#include <QApplication>
#include <QEvent>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QDebug>

MClientEventHandler::MClientEventHandler(QObject* parent) : QObject(parent) {
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


// Receive the PluginSession reference upon load
void MClientEventHandler::setPluginSession(PluginSession *ps) {
  _pluginSession = ps;
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
