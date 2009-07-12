#include "PluginManager.h"

#include "MClientPluginInterface.h"
#include "PluginConfigWidget.h"
#include "PluginEntry.h"
#include "PluginSession.h"

#include "CommandProcessor.h"
#include "ConfigManager.h"
#include "MainWindow.h"

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


PluginManager::PluginManager(QObject *parent) : QObject(parent) {
    _configManager = ConfigManager::instance();
    _mainWindow = MainWindow::instance(this);

    qDebug() << "* All singletons created!";

    /** Connect Other Necessary Objects */
    connect(this, SIGNAL(doneLoading()), _mainWindow, SLOT(start()));
    connect(_mainWindow, SIGNAL(startSession(const QString&)),
	    this, SLOT(initSession(const QString&)));
    connect(_mainWindow, SIGNAL(stopSession(const QString&)),
	    this, SLOT(stopSession(const QString&)));

    qDebug() << "* PluginManager is reading settings...";
    QHash<QString, QString> *hash = getConfig()->applicationSettings();
    
    QDateTime indexMod = QDateTime::fromString(hash->value("mClient/plugins/indexed"));
    _pluginDir = hash->value("mClient/plugins/path", "plugins");

    // Move into the plugins directory
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cd(_pluginDir);

    // Identify if there is something newer in the directory than the
    // index's generation date
    bool reIndex = false;
    foreach(QString fileName, pluginsDir.entryList(QDir::Files)) {
      QDateTime pluginMod = 
	QFileInfo(pluginsDir.absoluteFilePath(fileName)).lastModified();
      //qDebug() << fileName << pluginMod << "vs" << indexMod;
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

    qDebug() << "* Found profiles" << _configManager->profileNames();

    qDebug() << "PluginManager created with thread:" << this->thread();
    emit doneLoading();
}


PluginManager::~PluginManager() {

  // Remove entries in the available plugins list
  foreach(PluginEntry *pe, _availablePlugins) {
    delete pe;
  }

  _pinstance = 0;
  qDebug() << "* PluginManager destroyed";
}


void PluginManager::customEvent(QEvent*) {
//   MClientEvent* me = static_cast<MClientEvent*>(e);
//   //qDebug() << "* copying posted event with payload" << me->payload();
  
//   bool found = false;
//   foreach (QString s, me->dataTypes()) {
//     // Iterate through all the data types
//     //qDebug() << "* finding data type" << s << "out of" << me->dataTypes();
    
//     QMultiHash<QString, QPluginLoader*>::iterator it = _pluginTypes.find(s);
    
//     while (it != _pluginTypes.end() && it.key() == s) {
//       MClientEvent* nme = new MClientEvent(*me);
//       //qDebug() << "* copied payload to" << nme->payload();
//       // Need to make a copy, since the original event
//       // will be deleted when this function returns
//       qDebug() << "* posting" << nme->dataTypes() << "to" 
// 	       << it.value()->instance() << "with" << me->payload();
      
//       // Post the event
//       QApplication::postEvent(it.value()->instance(), nme);
//       found = true;
      
//       ++it; // Iterate
//     }
//   }
//   if (!found) qWarning() << "* No plugins accepted data types"
// 			 << me->dataTypes();
  
}


void PluginManager::configure() { 
  qDebug() << "! ConfigWidget not available, needs rewrite!";
  //if(!_configWidget) _configWidget = new PluginConfigWidget(_availablePlugins);
  //if(!_configWidget->isVisible()) _configWidget->show();
}


bool PluginManager::indexPlugins() {
  QDir pluginsDir = QDir(qApp->applicationDirPath());
  pluginsDir.cd(_pluginDir);
  
  //qDebug() << "Files: " << pluginsDir.entryList(QDir::Files);
  
  PluginEntry* e = 0;
  foreach(QString fileName, pluginsDir.entryList(QDir::Files)) {

    // See if it is a library
    if(!QLibrary::isLibrary(fileName)) {
      qWarning() << "! ERROR: " << fileName << "is not a library!";
      continue;
    }
    
    // Load the plugin
    QPluginLoader* loader = new QPluginLoader(pluginsDir
					      .absoluteFilePath(fileName));
    if(!loader->load()) {
      qCritical() << "! ERROR: Couldn't load" << fileName
		  << loader->errorString();
      continue;
    }
    
    // Cast it
    MClientPluginInterface* pi = 
      qobject_cast<MClientPluginInterface*>(loader->instance());
    if(!pi) {
      qCritical() << "! ERROR: Couldn't cast to interface" << fileName
		  << loader->errorString();
      continue;
    }
    
    // Put its info in memory
    e = new PluginEntry();
    e->shortName(pi->shortName());
    e->longName(pi->longName());
    e->libName(fileName);
    QHash<QString, int>::const_iterator it = pi->implemented().begin();
    for(; it!=pi->implemented().end(); ++it) {
      e->addAPI(it.key(), it.value());
    }
    
    // Insert the plugin, and read its settings
    _availablePlugins.insert(e->shortName(), e);
    getConfig()->readPluginSettings(e->shortName());
    
    loader->unload();
  }
  // Write the list of PluginEntrys to disk
  return writePluginIndex();
}


bool PluginManager::writePluginIndex() {
    QHash<QString, QString> *hash = getConfig()->applicationSettings();
    
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
    return true;
}


bool PluginManager::readPluginIndex() {
    // Read in the plugin db xml into PluginEntrys
    QHash<QString, QString> *hash = getConfig()->applicationSettings();
    
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
      _availablePlugins.insert(e->shortName(), e);
      getConfig()->readPluginSettings(e->shortName());
    }
    groups.removeLast(); /* plugins */
    groups.removeLast(); /* mClient */

    qDebug() << "* plugin index read";
    return true;
}


void PluginManager::initSession(const QString &s) {
  if (!_pluginSessions.contains(s)) {
    PluginSession *ps = new PluginSession(s, this);
    _pluginSessions.insert(s, ps);
    ps->start(QThread::TimeCriticalPriority);

  }
  else {
    qDebug() << "* Session" << s << "already exists";

  }
}


void PluginManager::initDisplay(PluginSession *ps) {
  // We now start/create the widgets (this is a slot from
  // PluginSession) in the main thread.
  ps->initDisplay();

}


void PluginManager::stopSession(const QString &s) {
  if (_pluginSessions.contains(s)) {
    PluginSession *ps = _pluginSessions[s];
    _pluginSessions.remove(s);
    ps->stopSession();
    ps->deleteLater();
    
  }
  else {
    qDebug() << "* Cannot stop session" << s << "because it does not exist";

  }
}
