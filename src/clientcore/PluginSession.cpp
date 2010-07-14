#include "PluginSession.h"

// World
#include "ConfigManager.h"
#include "PluginManager.h"
#include "MainWindow.h"

// Session
#include "CommandProcessor.h"
#include "ActionManager.h"
#include "AliasManager.h"

#include "MClientPluginInterface.h"
#include "PluginEntry.h"
#include "ConfigEntry.h"

#include "MClientEventHandler.h"

#include "MClientEngineEvent.h"
#include "MClientEvent.h"

#include <QApplication>
#include <QHash>
#include <QDir>
#include <QPluginLoader>
#include <QDebug>
#include <QVariant>

PluginSession::PluginSession(const QString &s, PluginManager *pm)
  : AbstractPluginSession(pm), _session(s), _pluginManager(pm) {

  // This is usually mume
  _mume = true;

  // Create alias and action managers
  _aliasManager = new AliasManager(this);
  _actionManager = new ActionManager(this);

  // Load aliases and actions (temporarily uses plugin code)
  getManager()->getConfig()->readPluginSettings(_session, "alias");
  getManager()->getConfig()->readPluginSettings(_session, "action");
  _aliasManager->loadSettings(*getManager()->
			      getConfig()->
			      pluginSettings(_session, "alias")->hash());
  _actionManager->loadSettings(*getManager()->
			       getConfig()->
			       pluginSettings(_session, "action")->hash());

  // Create the command processor
  _commandProcessor = new CommandProcessor(this);
  _commandProcessor->start();
  connect(_commandProcessor, SIGNAL(quit()),
	  _pluginManager->getMainWindow(), SLOT(close()));

  // Start the session in another thread to allow for widgets to be created
  connect(this, SIGNAL(doneLoading(PluginSession *)),
	  _pluginManager->getMainWindow(),
	  SLOT(initDisplay(PluginSession *)));
  connect(_pluginManager->getMainWindow(), SIGNAL(doneLoading()),
	  SLOT(doneLoading()));

  qDebug() << "* PluginSession" << _session
	   << "created with thread:" << QThread::currentThread();
}


PluginSession::~PluginSession() {
  /* This is unecessary because the plugin might still be in use by other sessions, right?
  // Unload all plugins 
  foreach(QPluginLoader *pm, _loadedPlugins) {
    delete pm->instance();
    pm->unload();
    delete pm;
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
  startSession();

  qDebug() << "* PluginSession" << _session
	   << "is running with thread:" << QThread::currentThread();
  emit doneLoading(this);
  exec();
}


void PluginSession::loadAllPlugins() {
  // Get the needed plugins for this profile
  SettingsHash *hash = getManager()->getConfig()->profileSettings(_session)->hash();

  QStringList pluginsToLoad;
  int pluginsSize = hash->value("profile/plugins/size", 0).toInt();
  for (int i = 0; i < pluginsSize; ++i) {
    // Add the plugin to the list of those to be loaded
    QString pluginName(hash->value("profile/plugin/"+
				   QString::number(i+1)+
				   "/name").toString());    
    pluginsToLoad << pluginName;
  }
  
  // Get a hash of the available plugins
  const QHash<QString, PluginEntry>& availablePlugins = _pluginManager->getAvailablePlugins();

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
	
	const PluginEntry& pe = availablePlugins.value(s);
	if (!loadPlugin(pe.libName())) {
	  qCritical() << "! Could not load plugin" << pe.shortName();
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
    qCritical() << "! Available plugins are:" << availablePlugins.keys();
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
    foreach(const PluginEntry& pe, _pluginManager->getAvailablePlugins()) {

      int version = pe.version(it.key());
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
		 << "satisfied by" << pe.shortName();

	if (_loadedPlugins.find(pe.shortName()) == _loadedPlugins.end()) {

	  qDebug() << "Attempting to load dependency" << pe.libName();
	  if (!loadPlugin(pe.libName())) {

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
      iPlugin->startSession(this);
      
      // Receive the event handler
      MClientEventHandler* eventHandler = iPlugin->getEventHandler(_session);

      if (!eventHandler) {
	qWarning() << "* eventHandler was invalid for " << iPlugin->shortName();
	continue;
      }

      // Insert datatypes this plugin wants into hash
      if(!iPlugin->receivesDataTypes().isEmpty()) {
	/*
	qDebug() << ">> receiving for " << iPlugin->shortName()
		 << "types" << iPlugin->receivesDataTypes();
	*/
	
	QHash<QString, int> hash = iPlugin->receivesDataTypes();
	QHash<QString, int>::const_iterator i = hash.constBegin();
	while (i != hash.constEnd()) {
	  _receivesTypes[i.key()].insertMulti(i.value(), eventHandler);
	  ++i;
	}
      }

      // Register the commands
      _commandProcessor->registerCommand(iPlugin->shortName(),
					 iPlugin->commandEntries());
    }
  }

  qDebug() << "* Populating event chains...";
  // Identify the head of the event handler command chain
  QHash<QString, QMultiMap<int, MClientEventHandler*> >::const_iterator i
    = _receivesTypes.constBegin();
  while (i != _receivesTypes.constEnd()) {
    MClientEventHandler *previous = NULL;
    QMultiMap<int, MClientEventHandler*> map = i.value();
    QMultiMap<int, MClientEventHandler*>::const_iterator j = map.constBegin();
    while (j != map.constEnd()) {
      // If we start at the front then this is the head
      if (previous == NULL) _receivesType.insert(i.key(), j.value());
      else previous->setNextHandler(i.key(), j.value());

      // Keep track of the previously seen event handler
      previous = j.value();
      ++j;
    }
    // Update the tail of the command chain
    previous->setNextHandler(i.key(), NULL);
    ++i;
  }
}


void PluginSession::stopSession() {
  qDebug() << "* PluginSession" << _session << "is stopping...";

  // Save profile settings
  getManager()->getConfig()->writeProfileSettings(_session);

  foreach(QPluginLoader *pl, _loadedPlugins) {
    MClientPluginInterface *pi
      = qobject_cast<MClientPluginInterface*>(pl->instance());
    if (pi) {
      // TODO
      //pi->saveSettings();
      qDebug() << "* sending stop to" << pi->shortName();
      pi->stopSession(_session);
    }
  }

  qDebug() << "* All" << _session << "plugins have been stopped.";
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
      
      QHash<QString, MClientEventHandler*>::iterator it = _receivesType.find(s);
      while (it != _receivesType.end() && it.key() == s) {
	MClientEvent* nme = new MClientEvent(*me);
	//qDebug() << "* copied payload to" << nme->payload();
	// Need to make a copy, since the original event
	// will be deleted when this function returns
	/*
	qDebug() << "* posting" << nme->dataTypes() << "to" 
		 << it.value() << "with" << me->payload();
	*/
	
	// Post the event
	QCoreApplication::postEvent(it.value(), nme);
	found = true;
	
	++it; // Iterate
      }
    }

    // TODO: Improve this somehow. This is very hack-ish.
    if (me->dataTypes().contains("SocketConnected") ||
	me->dataTypes().contains("SocketDisconnected") ||
	me->dataTypes().contains("UnlockProcessor")) {
      MClientEvent* nme = new MClientEvent(*me);
      QCoreApplication::postEvent(_commandProcessor, nme);
      nme = new MClientEvent(*me);
      qDebug() << "* posting to CommandProcessor";
      found = true;
    }
    
    if (!found) {
      if (me->dataTypes().contains("XMLAll")) {
	MClientEvent* nme = new MClientEvent(*me);
	QCoreApplication::postEvent(_commandProcessor, nme);
	nme = new MClientEvent(*me);
	qDebug() << "* posting to CommandProcessor";

      }
      else if (me->dataTypes().contains("MMapperInput")) {
	// HACK to allow client to run without MMapper
	QHash<QString, MClientEventHandler*>::iterator it
	  = _receivesType.find("SocketWriteData");
	while (it != _receivesType.end() && it.key() == "SocketWriteData") {
	  MClientEvent* nme = new MClientEvent(*me);
	  QCoreApplication::postEvent(it.value(), nme);
	  ++it; // Iterate
	}
      }
      else qWarning() << "! No plugins accepted data types" << me->dataTypes();
    }
    
  }
}


ConfigEntry* PluginSession::retrievePluginSettings(const QString &pluginName) const {
  return _pluginManager->getConfig()->pluginSettings(_session, pluginName);
}

void PluginSession::doneLoading() {
  QVariant *payload = new QVariant();
  QStringList sl("DoneLoading");
  MClientEventData *med = new MClientEventData(payload, sl, _session);
  MClientEvent* me = new MClientEvent(med);
  customEvent(me);
}
