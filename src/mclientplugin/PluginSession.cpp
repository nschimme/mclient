#include "PluginSession.h"

#include "ConfigManager.h"
#include "PluginManager.h"
#include "MainWindow.h"

#include "MClientDisplayInterface.h"
#include "MClientPluginInterface.h"
#include "PluginEntry.h"

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

  // To transfer widgets to the MainWindow we use the following lines
  // TODO: rewrite this to send the MClientDisplayInterface object
  qRegisterMetaType<QList<QPair<int,QWidget*> > >
    ("QList<QPair<int,QWidget*> >");
  connect(this, SIGNAL(sendWidgets(const QList< QPair<int, QWidget*> >&)),
	  _pluginManager->getMainWindow(),
	  SLOT(receiveWidgets(const QList< QPair<int, QWidget*> >&)));

  // Start the session in another thread to allow for widgets to be created
  connect(this, SIGNAL(doneLoading(PluginSession*)),
	  _pluginManager, SLOT(startSession(PluginSession*)));

  qDebug() << "* PluginSession" << _session
	   << "created with thread:" << this->thread();
}


PluginSession::~PluginSession() {
  quit();
  wait();

  // Unload all plugins
  foreach(QPluginLoader *pm, _loadedPlugins) {
    //delete pm->instance();
    pm->unload();
    //delete pm;
  }

  qDebug() << "* PluginSession " << _session << "destroyed";
}


// We want to start an event loop in a separate thread to handle plugins
void PluginSession::run() {
  loadAllPlugins();
  qDebug() << "* PluginSession" << _session
	   << "is running with thread:" << this->thread();
  emit doneLoading(this);
  exec();
}


void PluginSession::loadAllPlugins() {
  // Get the available plugins for this profile
  QStringList pluginsToLoad
    = _pluginManager->getConfig()->profilePlugins(_session);

  // Get a hash of the available plugins
  const QHash<QString, PluginEntry*> availablePlugins
    = _pluginManager->getAvailablePlugins();

  qDebug() << "* PluginSession" << _session
	   << " needs plugins" << pluginsToLoad;

  // Load the plugins in question
  bool foundAllPlugins = true;
  foreach(QString s, pluginsToLoad) {
    if (availablePlugins.contains(s)) {

      // Make sure the plugin wasn't already loaded as a dependency
      if (!_loadedPlugins.contains(s)) {

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


const bool PluginSession::loadPlugin(const QString& libName) {
  qDebug() << "loadPlugin call for" << libName
	   << "for session" << _session;
  
  QString fileName = _pluginManager->getPluginDir() + "/" + libName;
  
  // Try to load plugin from file
  QPluginLoader* loader = new QPluginLoader(fileName);
  // NOTE: can't have 'this' as parent because PluginManager is in a
  // different thread than its own.
  if(!loader->load()) {
    qWarning() << "! Failed to load plugin at" << fileName;
    delete loader;
    return false;
    
  } else {
    QObject* plugin = loader->instance();
    
    // Is it the correct kind of plugin?
    if(!qobject_cast<MClientPluginInterface*>(plugin)) {
      qWarning() << "! Plugin in file" << fileName
		 << "does not implement interface";
      loader->unload();
      delete loader;
      return false;
      
    } else {
      MClientPluginInterface* iPlugin
	= qobject_cast<MClientPluginInterface*>(loader->instance());

      // Check dependencies
      if (!checkDependencies(iPlugin))
	return false;
      
      // Insert shortname for this plugin into hash
      _loadedPlugins.insert(iPlugin->shortName(),loader);
      
      // insert APIs for this plugin into hash
      QHash<QString, int> apis = iPlugin->implemented();
      QHash<QString, int>::iterator jt = apis.begin();
      for(jt; jt!=apis.end(); ++jt) {
	_pluginAPIs.insert(jt.key(), loader);
      }
      
      // Insert datatypes this plugin wants into hash
      if(!iPlugin->receivesDataTypes().isEmpty()) {
	qDebug() << ">> receiving for " << iPlugin->shortName()
		 << "types" << iPlugin->receivesDataTypes();
	foreach(QString s, iPlugin->receivesDataTypes())
	  _receivesTypes.insert(s, loader);
      }

      // Insert datatypes this plugin wants into hash
      if(!iPlugin->deliversDataTypes().isEmpty()) {
	qDebug() << "<< delivering for" << iPlugin->shortName()
		 << "types" << iPlugin->deliversDataTypes();
	foreach(QString s, iPlugin->deliversDataTypes())
	  _deliversTypes.insert(loader, s);
      }

      // The plugin will need PluginSession's reference to send events
      iPlugin->setPluginSession(this);
      
      // Each plugin takes care of its own settings
      iPlugin->loadSettings();
      qDebug() << "Successfully loaded plugin" << iPlugin->shortName();
      
    } // casted
  } // loaded
  
  return true;
}


const bool PluginSession::checkDependencies(MClientPluginInterface *iPlugin) {
  QHash<QString, int> deps = iPlugin->dependencies();

  // Check dependencies
  QHash<QString, int>::iterator it = deps.begin();
  for (it; it != deps.end(); ++it) {

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
  
  postDataTypes();

  QList< QPair<int, QWidget*> > widgetList;

  foreach(QPluginLoader* pl,_loadedPlugins) {
    MClientPluginInterface* pi;
    pi = qobject_cast<MClientPluginInterface*>(pl->instance());
    if(pi) {
      pi->startSession(_session);
      
      MClientDisplayInterface* pd
	= qobject_cast<MClientDisplayInterface*>(pl->instance());
      if (pd) {
	qDebug() << "* Found a DISPLAY plugin! " << pi->shortName();
	
	
	qDebug() << "* Display Locations for"
		 << pi->shortName() << pd->displayLocations();
	
	QPair<int, QWidget*> pair(pd->displayLocations(),
				  pd->getWidget(_session));
	widgetList.append(pair);
      }
    }
  }
  
  if (!widgetList.isEmpty())
    emit sendWidgets(widgetList);
  else
    qCritical() << "! No widgets to use as a display!";
  
}


void PluginSession::stopSession() {
  foreach(QPluginLoader* pl,_loadedPlugins) {
    MClientPluginInterface* pi;
    pi = qobject_cast<MClientPluginInterface*>(pl->instance());
    if(pi) {
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
      
      QMultiHash<QString, QPluginLoader*>::iterator it
	= _receivesTypes.find(s);
      while (it != _receivesTypes.end() && it.key() == s) {
	MClientEvent* nme = new MClientEvent(*me);
	//qDebug() << "* copied payload to" << nme->payload();
	// Need to make a copy, since the original event
	// will be deleted when this function returns
	qDebug() << "* posting" << nme->dataTypes() << "to" 
		 << it.value()->instance() << "with" << me->payload();
	
	// Post the event
	QApplication::postEvent(it.value()->instance(), nme);
	found = true;
	
	++it; // Iterate
      }
    }
    if (!found)
      qWarning() << "! No plugins accepted data types" << me->dataTypes();
    
  }
}


void PluginSession::postDataTypes() {
  // Go through each unique plugin in this hash
  QList<QPluginLoader*> list = _deliversTypes.uniqueKeys();
  qDebug() << "postDataTypes has:" << list;
  for (int i = 0; i < list.size(); ++i) {
    QPluginLoader *iPlugin = list.at(i); // the current plugin

    qDebug() << "looking at" << iPlugin->instance() << i;

    // Hash to be delivered to the current plugin
    QHash<QString, QVariant> hash;
    
    // Iterate through each type that this plugin delivers
    QMultiHash<QPluginLoader*, QString>::iterator j
      = _deliversTypes.find(iPlugin);
    for (j; j != _deliversTypes.end() && j.key() == iPlugin; ++j) {
      QString dataType = j.value(); // the dataType
      QList<QPluginLoader*> rList = _receivesTypes.values(dataType);

      foreach(QPluginLoader *pl, rList)
	hash.insertMulti(dataType,
			 qVariantFromValue(static_cast<QObject*>
					   (pl->instance())));
    }

    if (!hash.isEmpty()) {
      // Create the engineEvent
      QVariant *payload = new QVariant(hash);
      MClientEngineEvent *ee
	= new MClientEngineEvent(new MClientEventData(payload),
				 EE_DATATYPE_UPDATE,
				 _session);
      
      // Post the event to the current plugin
      //QApplication::postEvent(iPlugin->instance(), ee);
      qDebug() << ">> delivering receiving types to" << iPlugin->instance()
	       << hash;
    }
  }
  qDebug() << "Done delivering types!";
}
