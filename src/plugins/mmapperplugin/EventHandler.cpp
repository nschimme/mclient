#include "EventHandler.h"

#include <QEvent>
#include <QVariant>
#include <QDebug>
#include <QCoreApplication> // for sending command

#include "MClientEvent.h"

#include "PluginSession.h"
#include "CommandProcessor.h"

EventHandler::EventHandler(QObject* parent) : MClientEventHandler(parent) {
}


EventHandler::~EventHandler() {
  qDebug() << "* removed MMapperPlugin for session"
	   << _pluginSession->session();
}


void EventHandler::customEvent(QEvent *e) {
  if(e->type() == 10000)
    engineEvent(e);

  else {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    foreach(QString s, me->dataTypes()) {
      if (s.startsWith("X")) {
	if (s.startsWith("XMLNone")) {
	  emit mudOutput(me->payload()->toString());
	  
	} else if (s.startsWith("XMLName")) {
	  emit name(me->payload()->toString());
	  
	} else if (s.startsWith("XMLDescription")) {
	  emit description(me->payload()->toString());
	  
	} else if (s.startsWith("XMLDynamicDescription")) {
	  emit dynamicDescription(me->payload()->toString());
	  
	} else if (s.startsWith("XMLExits")) {
	  emit exits(me->payload()->toString());
      
	} else if (s.startsWith("XMLPrompt")) {
	  emit prompt(me->payload()->toString());

	} else if (s.startsWith("XMLMove")) {
	  emit move(me->payload()->toString());
	  
	} else if (s.startsWith("XMLTerrain")) {
	  emit terrain(me->payload()->toString());
	  qDebug() << "! MMapperPlugin got XMLTerrain, is this needed?";

	}

      }
      else if (s.startsWith("M")) {
	if(s.startsWith("MMapperInput")) {
	  emit userInput(me->payload()->toString());
	  
	} else if (s.startsWith("MMapperLoadMap")) {
	  QString arguments = me->payload()->toString();
	  if (arguments.isEmpty())
	    displayMessage("#no file specified\n");
	  else if (arguments == "!") // hack
	    emit loadFile("/mnt/games/powwow/archive/arda.old.mm2");
	  else
	    emit loadFile(arguments);
	  
	}
      }
      else if (s.startsWith("S")) {
	if (s.startsWith("SocketConnected")) {
	  emit onPlayMode();

	}
	else if (s.startsWith("SocketDisconnected")) {
	  emit onOfflineMode();

	}

      }
    }
  }
}


void EventHandler::displayMessage(const QString& message) {
  QVariant* qv = new QVariant(message);
  QStringList sl("DisplayData");
  postSession(qv, sl);
}


void EventHandler::log(const QString& message, const QString& s) {
  qDebug() << "* MMapperPlugin[" << s << "]: " << message;
}
