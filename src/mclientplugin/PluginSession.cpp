#include "PluginSession.h"

// World
#include "ConfigManager.h"
#include "PluginManager.h"
#include "MainWindow.h"

// Session
#include "CommandProcessor.h"
#include "ActionManager.h"
#include "AliasManager.h"

#include "MClientDisplayInterface.h"
#include "MClientPluginInterface.h"
#include "PluginEntry.h"

#include "MClientEventHandler.h"

#include "MClientEngineEvent.h"
#include "MClientEvent.h"

#include <QApplication>
#include <QHash>
#include <QDir>
#include <QPluginLoader>
#include <QDebug>

PluginSession::PluginSession(const QString &s, PluginManager *pm,
			     QObject *parent)
  : QThread(parent), _session(s), _pluginManager(pm) {

  // Create alias and action managers
  _aliasManager = new AliasManager;
  _actionManager = new ActionManager;

  // Create the command processor
  _commandProcessor = new CommandProcessor(this);
  connect(_commandProcessor, SIGNAL(quit()),
	  _pluginManager->getMainWindow(), SLOT(close()));

  // To transfer widgets to the MainWindow we use the following lines
  // TODO: rewrite this to send the MClientDisplayInterface object
  qRegisterMetaType<QList<QPair<int,QWidget*> > >
    ("QList<QPair<int,QWidget*> >");
  connect(this, SIGNAL(sendWidgets(const QList< QPair<int, QWidget*> >&)),
	  _pluginManager->getMainWindow(),
	  SLOT(receiveWidgets(const QList< QPair<int, QWidget*> >&)));

  // Start the session in another thread to allow for widgets to be created
  connect(this, SIGNAL(doneLoading(PluginSession*)),
	  _pluginManager, SLOT(initDisplay(PluginSession*)));

  qDebug() << "* PluginSession" << _session
	   << "created with thread:" << this->thread();
}


PluginSession::~PluginSession() {
  /*
  // Unload all plugins 
  foreach(QPluginLoader *pm, _loadedPlugins) {
    //delete pm->instance();
    pm->unload();
    //delete pm;
  }
  */

  exit();
  wait();
  deleteLater();
  qDebug() << "* PluginSession" << _session << "destroyed";
}


// We want to start an event loop in a separate thread to handle plugins
void PluginSession::run() {
  loadAllPlugins();
  // Send the receiving plugins to each plugin which delivers
  postReceivingPlugins();
  startSession();

  qDebug() << "* PluginSession" << _session
	   << "is running with thread:" << this->thread();
  emit doneLoading(this);
  exec();
}


void PluginSession::loadAllPlugins() {
  // Get the needed plugins for this profile
  QHash<QString, QString> *hash
    = getManager()->getConfig()->profileSettings(_session);  

  QStringList pluginsToLoad;
  int pluginsSize = hash->value("profile/plugins/size", 0).toInt();
  for (int i = 0; i < pluginsSize; ++i) {
    // Add the plugin to the list of those to be loaded
    QString pluginName(hash->value("profile/plugins/"+
				   QString::number(i+1)+
				   "/name"));    
    pluginsToLoad << pluginName;
  }
  
  // Get a hash of the available plugins
  const QHash<QString, PluginEntry*> availablePlugins
    = _pluginManager->getAvailablePlugins();

  if (pluginsToLoad.isEmpty()) {
    pluginsToLoad = availablePlugins.keys();
    qDebug() << "! Profile config was blank; all plugins will be loaded!";
  }

  qDebug() << "* PluginSession" << _session
	   << "needs plugins" << pluginsToLoad;

  // Load the plugins in question
  bool foundAllPlugins = true;
  foreach(QString s, pluginsToLoad) {
    if (availablePlugins.contains(s)) {

      // Make sure the plugin wasn't already loaded as a dependency
      if (!_loadedPlugins.contains(s)) {

	// Load the plugin's configuration
	getManager()->getConfig()->readPluginSettings(_session, s);
	
	PluginEntry *pe = availablePlugins.value(s);
	if (!loadPlugin(pe->libName())) {
	  qCritical() << "! Could not load plugin" << pe->shortName();
	  // TODO: what should happen if we cannot load a plugin?
	  
	}
      }
    }
    else {
      qCritical() << "! Unable to find plugin" << s
		  << "for session" << _session;
      foundAllPlugins = false;
    }
  }
  if (!foundAllPlugins)
    qCritical() << "! Available plugins are:" << availablePlugins;
}


bool PluginSession::loadPlugin(const QString& libName) {
  qDebug() << "loadPlugin call for" << libName
	   << "for session" << _session;
  
  QString fileName = _pluginManager->getConfig()->
    getPluginPath() + "/" + libName;
  
  // Try to load plugin from file
  QPluginLoader* loader = new QPluginLoader(fileName);
  // NOTE: can't have 'this' as parent because PluginSession is in a
  // different thread than its own.
  if(!loader->load()) {
    qWarning() << "! Failed to load plugin at" << fileName;
    delete loader;
    return false;
    
  } else {
    MClientPluginInterface* iPlugin
      = qobject_cast<MClientPluginInterface*>(loader->instance());

    // Is it the correct kind of plugin?
    if(!iPlugin) {
      qWarning() << "! Plugin in file" << fileName
		 << "does not implement interface";
      loader->unload();
      delete loader;
      return false;
      
    } else {

      // Check dependencies
      if (!checkDependencies(iPlugin))
	return false;

      // insert APIs for this plugin into hash
      QHash<QString, int> apis = iPlugin->implemented();
      QHash<QString, int>::iterator jt = apis.begin();
      for(; jt!=apis.end(); ++jt) {
	_pluginAPIs.insert(jt.key(), loader);
      }
      
      // Insert shortname for this plugin into hash
      _loadedPlugins.insert(iPlugin->shortName(), loader);
      qDebug() << "Successfully loaded plugin" << iPlugin->shortName();

    } // casted
  } // loaded
  
  return true;
}


bool PluginSession::checkDependencies(MClientPluginInterface *iPlugin) {
  QHash<QString, int> deps = iPlugin->dependencies();

  // Check dependencies
  QHash<QString, int>::iterator it = deps.begin();
  for (; it != deps.end(); ++it) {

    // Ask what APIs each plugin implements and compare versions
    foreach(PluginEntry *pe, _pluginManager->getAvailablePlugins()) {

      int version = pe->version(it.key());
      if (version) {

	// Found one of the APIs, so check the version
	// We want the version of this plugin to be <= the version of
	// the one we're checking against.
	// Incompatible updates should change the API name.
	if (version > it.value()) {
	  qDebug() << "Dependency version mismatch!";
	  continue;
	}
	
	qDebug() << "Dependency" << it.key() << it.value()
		 << "satisfied by" << pe->shortName();

	if (_loadedPlugins.find(pe->shortName()) == _loadedPlugins.end()) {

	  qDebug() << "Attempting to load dependency" << pe->libName();
	  if (!loadPlugin(pe->libName())) {

	    qWarning() << "! Could not load deps for" << it.key();
	    return false;
	    
	  }
	}
      }
    }    
  }
  return true;
}


void PluginSession::startSession() {
  // Go through each loaded plugin and start it
  foreach(QPluginLoader *loader, _loadedPlugins) {
    MClientPluginInterface* iPlugin
      = qobject_cast<MClientPluginInterface*>(loader->instance());
    
    if (iPlugin) {      
      // Start the sessions within the thread
      iPlugin->startSession(_session);
      
      // Receive the event handler
      MClientEventHandler* eventHandler = iPlugin->getEventHandler(_session);

      // The plugin will need PluginSession's reference to send events
      eventHandler->setPluginSession(this);

      // Each plugin takes care of its own settings
      iPlugin->loadSettings();
      
      // Insert datatypes this plugin wants into hash
      if(!iPlugin->receivesDataTypes().isEmpty()) {
	/*
	qDebug() << ">> receiving for " << iPlugin->shortName()
		 << "types" << iPlugin->receivesDataTypes();
	*/
	foreach(QString s, iPlugin->receivesDataTypes())
	  _receivesTypes.insert(s, eventHandler);
      }

      // Insert datatypes this plugin wants into hash
      if(!iPlugin->deliversDataTypes().isEmpty()) {
	/*
	qDebug() << "<< delivering for" << iPlugin->shortName()
		 << "types" << iPlugin->deliversDataTypes();
	*/
	foreach(QString s, iPlugin->deliversDataTypes())
	  _deliversTypes.insert(loader, s);
      }

      // Register the commands
      _commandProcessor->registerCommand(iPlugin->shortName(),
					 iPlugin->commandEntries());

    }
  }
}

void PluginSession::initDisplay() {
  // Create the widgets outside of the threads
  QList< QPair<int, QWidget*> > widgetList;

  foreach(QPluginLoader* pl,_loadedPlugins) {
    MClientDisplayInterface* pd
      = qobject_cast<MClientDisplayInterface*>(pl->instance());
    if (pd) {
      pd->initDisplay(_session);

      QPair<int, QWidget*> pair(pd->displayLocations(),
				pd->getWidget(_session));
      widgetList.append(pair);
    }
  }

  if (!widgetList.isEmpty())
    emit sendWidgets(widgetList);
  else
    qCritical() << "! No widgets to use as a display!";
}


void PluginSession::stopSession() {
  // Save profile settings
  getManager()->getConfig()->writeProfileSettings(_session);

  foreach(QPluginLoader *pl, _loadedPlugins) {
    MClientPluginInterface *pi
      = qobject_cast<MClientPluginInterface*>(pl->instance());
    if (pi) {
      pi->saveSettings();
      pi->stopSession(_session);
    }
  }

  qDebug() << "* All" << _session << "sessions have been stopped.";
}


const QPluginLoader* PluginSession::pluginWithAPI(const QString& api) const {
  QPluginLoader* plugin = _pluginAPIs.find(api).value();
  return plugin;
}


void PluginSession::customEvent(QEvent* e) {
  if (e->type() == 10000) {
    MClientEngineEvent* ee = static_cast<MClientEngineEvent*>(e);
    if (ee->dataType() == EE_MANAGER_POST) {

    }
    else if (ee->dataType() == EE_DATATYPE_UPDATE) {

    }
    
  }
  else if (e->type() == 10001) {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    //qDebug() << "* copying posted event with payload" << me->payload();
    
    bool found = false;
    foreach (QString s, me->dataTypes()) {
      // Iterate through all the data types
      //qDebug() << "* finding data type" << s << "out of" << me->dataTypes();
      
      QMultiHash<QString, MClientEventHandler*>::iterator it
	= _receivesTypes.find(s);
      while (it != _receivesTypes.end() && it.key() == s) {
	MClientEvent* nme = new MClientEvent(*me);
	//qDebug() << "* copied payload to" << nme->payload();
	// Need to make a copy, since the original event
	// will be deleted when this function returns
	qDebug() << "* posting" << nme->dataTypes() << "to" 
		 << it.value() << "with" << me->payload();
	
	// Post the event
	QCoreApplication::postEvent(it.value(), nme);
	found = true;
	
	++it; // Iterate
      }
    }
    if (!found) {
      if (me->dataTypes().contains("XMLAll")) {
	MClientEvent* nme = new MClientEvent(*me);
	QCoreApplication::postEvent(_commandProcessor->getAction(), nme);
	qDebug() << "* posting to CommandProcessor";
      }
    } else {
      qWarning() << "! No plugins accepted data types" << me->dataTypes();

    }
    
  }
}


void PluginSession::postReceivingPlugins() {
  /*
  // Go through each unique plugin in this hash
  QList<QPluginLoader*> list = _deliversTypes.uniqueKeys();

  for (int i = 0; i < list.size(); ++i) {
    QPluginLoader *pl = list.at(i); // the current plugin

    // Hash to be delivered to the current plugin
    QMultiHash<QString, QVariant> hash;
    
    // Iterate through each type that this plugin delivers
    QMultiHash<QPluginLoader*, QString>::iterator j
      = _deliversTypes.find(pl);
    for (; j != _deliversTypes.end() && j.key() == pl; ++j) {
      QString dataType = j.value(); // the dataType
      QList<MClientEventHandler*> receivesTypes
	= _receivesTypes.values(dataType);

      foreach(MClientEventHandler *rpl, receivesTypes)
	hash.insert(dataType, qVariantFromValue(rpl));

      // Hack to deliver output to CommandProcessor
      if (dataType == "XMLAll") {
	hash.insert(dataType,
		    qVariantFromValue(static_cast<QObject*>
				      (_commandProcessor)));
      }
      
    }

    if (!hash.isEmpty()) {
      // Create the engineEvent
      QVariant *payload = new QVariant(hash);
      MClientEngineEvent *ee
	= new MClientEngineEvent(new MClientEventData(payload, QStringList(),
						      _session),
				 EE_DATATYPE_UPDATE,
				 _session);

      // Post the event to the current plugin
      QCoreApplication::postEvent(pl->instance(), ee);

      MClientPluginInterface *pi
	= qobject_cast<MClientPluginInterface*>(pl->instance());
      qDebug() << ">> delivering receiving types to" << pi->shortName()
	       << hash.uniqueKeys();
    }
  }
  */

}
