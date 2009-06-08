#include "PluginManager.h"

#include "MClientDisplayInterface.h"
#include "MClientPluginInterface.h"
#include "MClientEvent.h"
#include "PluginConfigWidget.h"
#include "PluginEntry.h"

#include "ConfigManager.h"
#include "MainWindow.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QHash>
#include <QLibrary>
#include <QPluginLoader>
#include <QWidget>

PluginManager* PluginManager::_pinstance = 0;

PluginManager* PluginManager::instance() {
    if(!_pinstance) {
        _pinstance = new PluginManager();
    }
    
    return _pinstance;
}


void PluginManager::destroy() {
    quit();
    wait();
    delete this;
    _pinstance = 0;
}


// PluginManager::PluginManager(QWidget* display, QObject* io, QObject* filter, 
//         QObject* parent) : QThread(parent) {
//     _displayParent = display;
//     _ioParent = io;
//     _filterParent = filter;

//     _pluginDir = "./plugins";

//     qDebug() << "PluginManager created with thread:" << this->thread();
// }


PluginManager::PluginManager(QObject* parent) : QThread(parent) {
    QHash<QString, QString> *hash = ConfigManager::instance()->
      applicationSettings();
    
    QDateTime indexMod = QDateTime::fromString(hash->value("mClient/plugins/indexed"));
    _pluginDir = hash->value("mClient/plugins/path", "./plugins");

    // Move into the plugins directory
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cdUp();
    pluginsDir.cd(_pluginDir);

    // Identify if there is something newer in the directory than the
    // index's generation date
    bool reIndex = false;
    foreach(QString fileName, pluginsDir.entryList(QDir::Files)) {
      QDateTime pluginMod = 
	QFileInfo(pluginsDir.absoluteFilePath(fileName)).lastModified();
      if(indexMod < pluginMod) {
	reIndex = true;
	break;
      }
    }
    if(reIndex) {
      qDebug() << "* index is old and needs to be regenerated ("
	       << indexMod.toString() << ")";
      indexPlugins();
    } else {
      readPluginIndex();
    }

    // Load plugins using data in that file, which should now be accurate.
    _doneLoading = false;
    
    qDebug() << "PluginManager created with thread:" << this->thread();
}


PluginManager::~PluginManager() {

    // Remove entries in the available plugins list
    PluginEntry* pe;
    foreach(pe, _availablePlugins) {
        delete pe;
    }

    // Unload all plugins
    QPluginLoader* pm;
    foreach(pm, _loadedPlugins) {
        delete pm->instance();
        pm->unload();
        delete pm;
    }
}


void PluginManager::loadAllPlugins() {
    PluginEntry* pe;
    foreach(pe, _availablePlugins) {
        if(_loadedPlugins.find(pe->shortName()) == _loadedPlugins.end()) {
            bool loader = loadPlugin(pe->libName());
            if(!loader) {
                qDebug() << "Could not load plugin" << pe->shortName();

            } else {
            //qDebug() << "Successfully loaded plugin" << pe->shortName();
            }
        }
    }

    _doneLoading = true;

}


const bool PluginManager::loadPlugin(const QString& libName) {
    qDebug() << "loadPlugin call for" << libName;

    QString fileName = _pluginDir + "/" + libName;

    if(!QLibrary::isLibrary(fileName)) {
        // Error handling
        qWarning() << fileName << "is not a library!";
        return false;
    }
    
    // Try to load plugin from file
    QPluginLoader* loader = new QPluginLoader(fileName);
    // NOTE: can't have 'this' as parent because PluginManager is in a
    // different thread than its own.
    if(!loader->load()) {
        qWarning() << "failed to load plugin from file" << fileName;
        return false;

    } else {
        QObject* plugin = loader->instance();
        
        // Is it the correct kind of plugin?
        if(!qobject_cast<MClientPluginInterface*>(plugin)) {
            qWarning() << "plugin in file" << fileName 
                    << "does not implement MClientPluginInterface!";
            loader->unload();
            delete loader;
            return false;
        } else {
            MClientPluginInterface* iPlugin = 
                    qobject_cast<MClientPluginInterface*>(plugin);

            QHash<QString, int> deps = iPlugin->dependencies();
           
            if(!deps.isEmpty()) {

            // Check dependencies
            QHash<QString, int>::iterator it = deps.begin();
            for(it; it!=deps.end(); ++it) {
                // Ask what APIs each plugin implements and compare versions
                PluginEntry* pe;
                foreach(pe, _availablePlugins) {
                    int version = pe->version(it.key());
                    if(!version) {
                        /*
                        qDebug() << "Plugin" << pe->shortName() 
                                 << "does not provide an implementation of" 
                                 << it.key();
                        */
                    } else {

                        // Found one of the APIs, so check the version
                        // We want the version of this plugin to be <= the
                        // version of the one we're checking against.
                        // Incompatible updates should change the API name.
                        if(version > it.value()) {
                            qDebug() << "version mismatch!";
                            continue;
                        }

                        qDebug() << "Dependency" << it.key() << it.value()
                                 << "satisfied by" << pe->shortName();
                        if(_loadedPlugins.find(pe->shortName()) == 
                                _loadedPlugins.end()) {
                            qDebug() << "attempting to load dep" 
                                << pe->libName();
                            if(loadPlugin(//pluginsDir.absoluteFilePath(
                                            pe->libName())) {//) {
                                qDebug() << "succeeded!";
                    //        deps.erase(it);

                            } else {
                                qWarning() << "could not load deps for" 
                                    << it.key();
                                return false;
                            }
                        }
                    }
                }



            } // deps
            } // if has deps
           
            // Insert shortname for this plugin into hash
            _loadedPlugins.insert(iPlugin->shortName(),loader);

            // insert APIs for this plugin into hash
            QHash<QString, int> apis = iPlugin->implemented();
            QHash<QString, int>::iterator jt = apis.begin();
            for(jt; jt!=apis.end(); ++jt) {
                _pluginAPIs.insert(jt.key(), loader);
            }
           
            // Insert datatypes this plugin wants into hash
            if(!iPlugin->dataTypes().isEmpty()) {
	      QString s;
	      foreach(s, iPlugin->dataTypes()) {
		qDebug() << "* adding type" << s << "for" << iPlugin->shortName() << loader->instance();
		_pluginTypes.insert(s, loader);
	      }
            }

            // Each plugin takes care of its own settings
            iPlugin->loadSettings();
            qDebug() << "Successfully loaded plugin" << iPlugin->shortName();
        } // casted
    } // loaded
    
    return true; // never actually gets reached
}


const QPluginLoader* PluginManager::pluginWithAPI(const QString& api) const {

    QPluginLoader* plugin = _pluginAPIs.find(api).value();
    return plugin;
}


void PluginManager::customEvent(QEvent* e) {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    //qDebug() << "* copying posted event with payload" << me->payload();
  
    bool found = false;
    foreach (QString s, me->dataTypes()) {
      // Iterate through all the data types
      //qDebug() << "* finding data type" << s << "out of" << me->dataTypes();

      QMultiHash<QString, QPluginLoader*>::iterator it = _pluginTypes.find(s);
  
      while (it != _pluginTypes.end() && it.key() == s) {
	MClientEvent* nme = new MClientEvent(*me);
	//qDebug() << "* copied payload to" << nme->payload();
	// Need to make a copy, since the original event
	// will be deleted when this function returns
	qDebug() << "* posting" << nme->dataTypes() << "to" 
		 << it.value()->instance() << "with" << nme->payload();

	// Post the event
	QApplication::postEvent(it.value()->instance(), nme);
	found = true;

	++it; // Iterate
      }
    }

    if (!found) qWarning() << "* No plugins accepted data types"
			   << me->dataTypes();
}


// We want to start an event loop in a separate thread to handle plugins
void PluginManager::run() {
    loadAllPlugins();
    emit doneLoading();
    exec();
}



void PluginManager::configure() { 
    if(!_configWidget) _configWidget = new PluginConfigWidget(_loadedPlugins);
    if(!_configWidget->isVisible()) _configWidget->show();
}


void PluginManager::initSession(const QString &s) {
  QList< QPair<int, QWidget*> > widgetList;

    foreach(QPluginLoader* pl,_loadedPlugins) {
        MClientPluginInterface* pi;
        pi = qobject_cast<MClientPluginInterface*>(pl->instance());
        if(pi) {
	  pi->startSession(s);

	  if (pi->type() == DISPLAY_PLUGIN) {
	    qDebug() << "* Found a DISPLAY plugin! " << pi->shortName();

	    MClientDisplayInterface* pd;
	    pd = qobject_cast<MClientDisplayInterface*>(pl->instance());

	    qDebug() << "* Display Locations for"
		     << pi->shortName() << pd->displayLocations();

	    QPair<int, QWidget*> pair(pd->displayLocations(), pd->getWidget(s));
	    widgetList.append(pair);
	  }
        }
    }
    
    if (!widgetList.isEmpty())
      MainWindow::instance()->receiveWidgets(widgetList);

}

void PluginManager::stopSession(const QString &s) {
   foreach(QPluginLoader* pl,_loadedPlugins) {
        MClientPluginInterface* pi;
        pi = qobject_cast<MClientPluginInterface*>(pl->instance());
        if(pi) {
	  pi->stopSession(s);
	}
   }
   qDebug() << "* All" << s << "sessions have been stopped.";
}


const bool PluginManager::indexPlugins() {
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cdUp();
    pluginsDir.cd(_pluginDir);

    //qDebug() << "Files: " << pluginsDir.entryList(QDir::Files);

    PluginEntry* e = 0;
    foreach(QString fileName, pluginsDir.entryList(QDir::Files)) {
        //qDebug() << pluginsDir.absoluteFilePath(fileName);

        // Load the plugin
        QPluginLoader* loader = 
            new QPluginLoader(pluginsDir.absoluteFilePath(fileName));

        if(!loader->load()) {
	  qCritical() << "* ERROR: Couldn't load!" << fileName << loader->errorString();
            continue;
        }

        // Cast it
        MClientPluginInterface* pi = 
            qobject_cast<MClientPluginInterface*>(loader->instance());
        if(!pi) {
	    qCritical() << "* ERROR: Couldn't cast" << fileName << loader->errorString();
            continue;
        }
        
        // Put its info in memory
        e = new PluginEntry();
        e->shortName(pi->shortName());
        e->longName(pi->longName());
        e->libName(fileName);
        QHash<QString, int>::const_iterator it = pi->implemented().begin();
        for(it; it!=pi->implemented().end(); ++it) {
            e->addAPI(it.key(), it.value());
        }

	// Insert the plugin, and read its settings
        _availablePlugins.insert(e->libName(), e);
	ConfigManager::instance()->readPluginSettings(e->shortName());

        loader->unload();
    }
    // Write the list of PluginEntrys to disk
    return writePluginIndex();
}


const bool PluginManager::writePluginIndex() {
    QHash<QString, QString> *hash = ConfigManager::instance()->applicationSettings();
    
    QStringList groups;
    groups << "mClient" << "plugins";
    QList<PluginEntry*> plugins = _availablePlugins.values();
    hash->insert(groups.join("/")+"/size", QString::number(plugins.size()));
    hash->insert(groups.join("/")+"/path", _pluginDir);
    hash->insert(groups.join("/")+"/indexed",
		QDateTime(QDateTime::currentDateTime()).toString());
    for (int i = 0; i < plugins.size(); ++i) {
      PluginEntry *e = plugins.at(i);
      groups << QString::number(i+1); /* plugins/index */
      
      hash->insert(groups.join("/")+"/shortname", e->shortName());
      hash->insert(groups.join("/")+"/libname", e->libName());
      hash->insert(groups.join("/")+"/longname", e->longName());
      
      if (!e->apiList().isEmpty()) {
	groups << "api";
	QStringList api = e->apiList();
	hash->insert(groups.join("/")+"/size", QString::number(api.size()));
	for(int j = 0; j < api.size(); ++j) {
	  groups << QString::number(j); /* api/index */
	  hash->insert(groups.join("/")+"/name", api.at(j));
	  hash->insert(groups.join("/")+"/version",
		       QString::number(e->version(api.at(j))));
	  groups.removeLast(); /* api/index */
	}
	groups.removeLast(); /* api */
      }
      groups.removeLast(); /* plugins/index */
    }
    groups.removeLast(); /* plugins */
    groups.removeLast(); /* mClient */
    
    qDebug("* plugin index written");
}


const bool PluginManager::readPluginIndex() {
    // Read in the plugin db xml into PluginEntrys
    QHash<QString, QString> *hash = ConfigManager::instance()->applicationSettings();
    
    QStringList groups;
    groups << "mClient" << "plugins";
    QDateTime generated =
      QDateTime::fromString(hash->value(groups.join("/")+"/generated"));

    int pluginsSize = hash->value(groups.join("/")+"/size", 0).toInt();
    for (int i = 0; i < pluginsSize; ++i) {
      PluginEntry* e = new PluginEntry();
      groups << QString::number(i+1); /* plugins/index */
      e->libName(hash->value(groups.join("/")+"/libname"));
      e->longName(hash->value(groups.join("/")+"/longname"));
      e->shortName(hash->value(groups.join("/")+"/shortname"));

      groups << "api";
      int apiSize = hash->value(groups.join("/")+"/size", 0).toInt();
      for (int j = 0; j < apiSize; ++j) {
	groups << QString::number(j+1); /* api/index */
	QString name = hash->value(groups.join("/")+"/name");
	int version = hash->value(groups.join("/")+"/version").toInt();
	e->addAPI(name, version);
	groups.removeLast(); /* api/index */
      }
      groups.removeLast(); /* api */
      groups.removeLast(); /* plugins/index */

      // Insert the plugin, and read its settings
      _availablePlugins.insert(e->libName(), e);
      ConfigManager::instance()->readPluginSettings(e->shortName());
    }
    groups.removeLast(); /* plugins */
    groups.removeLast(); /* mClient */

    qDebug() << "* plugin index read";
    return true;
}
