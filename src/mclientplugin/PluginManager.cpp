#include "PluginManager.h"

#include "MClientDisplayInterface.h"
#include "MClientPluginInterface.h"
#include "MClientEvent.h"
#include "PluginConfigWidget.h"
#include "PluginEntry.h"

#include "MainWindow.h"

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QHash>
#include <QLibrary>
#include <QPluginLoader>
#include <QString>
#include <QtXml>
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


PluginManager::PluginManager(QWidget* display, QObject* io, QObject* filter, 
        QObject* parent) : QThread(parent) {
    _displayParent = display;
    _ioParent = io;
    _filterParent = filter;

    _pluginDir = "./plugins";

    // IS THIS EVER EVEN SEEN>???
    qDebug() << "PluginManager created with thread:" << this->thread();
}


PluginManager::PluginManager(QObject* parent) : QThread(parent) {
    _pluginDir = "./plugins";
    _pluginIndex = "plugindb.xml";

    // Create plugin index file if it doesn't exist, or re-index plugins if
    // the number of entries in the file differs from the number of plugins in
    // the plugin dir.  This should replace the hardcoded PluginEntries.
    
    // Does the index exist?  If so, load it.  If not, create it.
    if(!QFile::exists(_pluginIndex)) {
        qDebug() << "* index doesn't exist!";
        indexPlugins();
    } else {
        QDir pluginsDir = QDir(qApp->applicationDirPath());
        pluginsDir.cdUp();
        pluginsDir.cd(_pluginDir);

        QFileInfo finfo(_pluginIndex);//pluginsDir.absoluteFilePath(_pluginIndex));
        
        // Is the index up-to-date 
        // (i.e. newer than anything in the pluginsDir?)
        QDateTime indexMod = finfo.lastModified();
    
        bool reIndex = false;
        foreach(QString fileName, pluginsDir.entryList(QDir::Files)) {
            QDateTime pluginMod = 
                QFileInfo(pluginsDir.absoluteFilePath(fileName))
                    .lastModified();
            if(indexMod < pluginMod) {
                reIndex = true;
                break;
            }
        }
        if(reIndex) {
            qDebug() << "* index is old and needs to be regenerated";
            indexPlugins();
        } else {
            readPluginIndex();
        }
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
  
    QString s;
    foreach (s, me->dataTypes()) {
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
	
	++it; // Iterate
      }
    }
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
    foreach(QPluginLoader* pl,_loadedPlugins) {
        MClientPluginInterface* pi;
        pi = qobject_cast<MClientPluginInterface*>(pl->instance());
        if(pi) {
	  pi->startSession(s);
	  
	  /*
	  // Display type plugins need QWidget transfers
	  if (pi->type() == DISPLAY) {
	  */
	  if (pi->displayLocations() > 0 &&
	      ISNOTSET(pi->displayLocations(), DL_FLOAT)) {
	    qDebug() << "* Found a DISPLAY plugin! " << pi->shortName();
	    MClientDisplayInterface* pd;
	    pd = qobject_cast<MClientDisplayInterface*>(pl->instance());
	    MainWindow* mw = MainWindow::instance();
	    mw->receiveWidget(pd->getWidget(s));
	  }
        }
    }
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
	  qCritical() << "* ERROR: Couldn't load!" << fileName;
            continue;
        }

        // Cast it
        MClientPluginInterface* pi = 
            qobject_cast<MClientPluginInterface*>(loader->instance());
        if(!pi) {
	    qCritical() << "* ERROR: Couldn't cast" << fileName;
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
        //qDebug() << "indexed" << e->libName();
        _availablePlugins.insert(e->libName(), e);

        loader->unload();
    }
    // Write the list of PluginEntrys to disk
    return writePluginIndex();
}


const bool PluginManager::writePluginIndex() {
    QIODevice* device = new QFile(_pluginIndex);
    if(!device->open(QIODevice::WriteOnly)) {
        qCritical() << "Can't open file for writing:" << _pluginIndex;
        return false;
    }

    QXmlStreamWriter* xml = new QXmlStreamWriter(device);
    xml->setAutoFormatting(true);
    xml->writeStartDocument();
    xml->writeStartElement("index");
    
    //QHash<QString, PluginEntry*>::iterator it = _availablePlugins.begin();
    //for(it; it!=_availablePlugins.end(); ++it) {
    foreach(PluginEntry* e, _availablePlugins) {
        // Write it out as xml
        xml->writeStartElement("plugin");

        xml->writeStartElement("libname");
        xml->writeCharacters(e->libName());
        xml->writeEndElement();

        xml->writeStartElement("shortname");
        xml->writeCharacters(e->shortName());
        xml->writeEndElement();

        xml->writeStartElement("longname");
        xml->writeCharacters(e->longName());
        xml->writeEndElement();

        foreach(QString s, e->apiList()) {
            //qDebug() << s << e->apiList();
            xml->writeEmptyElement("api");
            xml->writeAttribute("name", s);
            xml->writeAttribute("version", QString::number(e->version(s)));
        }

        xml->writeEndElement(); // plugin
    }

    xml->writeEndElement(); // index
    xml->writeEndDocument();

    delete device;
    delete xml;

    qDebug("* index written");
}


const bool PluginManager::readPluginIndex() {
    // Read in the plugin db xml into PluginEntrys
    QIODevice* device = new QFile(_pluginIndex);
    if(!device->open(QIODevice::ReadOnly)) {
        qCritical() << "Can't open file for reading:" << _pluginIndex;
        return false;
    }

    QXmlStreamReader* xml = new QXmlStreamReader(device);

    PluginEntry* e = 0;
    while(!xml->atEnd()) {
        xml->readNext();

        if(xml->isEndElement()) {
            if(xml->name() == "plugin") {
                _availablePlugins.insert(e->libName(), e);
            }
        
        } else if(xml->isStartElement()) {
            if(xml->name() == "plugin") {
                e = new PluginEntry();
            
            } else if(xml->name() == "libname") {
                e->libName(xml->readElementText());

            } else if(xml->name() == "shortname") {
                e->shortName(xml->readElementText());

            } else if(xml->name() == "longname") {
                e->longName(xml->readElementText());

            } else if(xml->name() == "api") {
                QXmlStreamAttributes attr = xml->attributes();
                QString name = attr.value("name").toString();
                int version = attr.value("version").toString().toInt();
                e->addAPI(name, version);
            }
        }
    }
    return true;
}
