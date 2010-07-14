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
#include <QSettings>

PluginManager* PluginManager::_instance = 0;

PluginManager* PluginManager::getInstance() {
  if (!_instance) _instance = new PluginManager;
  Q_ASSERT(_instance != 0);
  return _instance;
}

PluginManager::PluginManager(QObject *parent) : QObject(parent) {
    _configManager = new ConfigManager(this);
    _mainWindow = new MainWindow(this);

    qDebug() << "* All major objects created!";

    /** Connect Other Necessary Objects */
    connect(this, SIGNAL(doneLoading()), _mainWindow, SLOT(start()));
    connect(_mainWindow, SIGNAL(startSession(const QString&)),
	    SLOT(initSession(const QString&)));
    connect(_mainWindow, SIGNAL(stopSession(const QString&)),
	    SLOT(stopSession(const QString&)));

    qDebug() << "* PluginManager is reading settings...";
    //QHash<QString, QVariant> *hash = getConfig()->applicationSettings();
    QSettings conf;
    
    QDateTime indexMod
      = QDateTime::fromString(conf.value("mClient/plugins/indexed")
			      .toString());

    // Move into the plugins directory
    QDir pluginsDir = QDir(qApp->applicationDirPath());
    pluginsDir.cd(getConfig()->getPluginPath());

    // Identify if there is something newer in the directory than the
    // index's generation date
    bool reIndex = true; // false; // HACK, ALWAYS REINDEX
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

    // Figure out which profiles we have
    _configManager->discoverProfiles();

    qDebug() << "PluginManager created with thread:" << this->thread();
    emit doneLoading();
}


PluginManager::~PluginManager() {
  qDebug() << "* PluginManager is destroying itself";

  foreach(PluginSession *ps, _pluginSessions) {
    stopSession(ps->session());
    delete ps;
  }

  // Remove entries in the available plugins list
  _availablePlugins.clear();

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
  pluginsDir.cd(getConfig()->getPluginPath());
  
  //qDebug() << "Files: " << pluginsDir.entryList(QDir::Files);
  
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
    PluginEntry e;
    e.shortName(pi->shortName());
    e.longName(pi->longName());
    e.libName(fileName);
    QHash<QString, int>::const_iterator it = pi->implemented().begin();
    for(; it!=pi->implemented().end(); ++it) {
      e.addAPI(it.key(), it.value());
    }
    
    // Insert the plugin as one of the available plugins
    _availablePlugins.insert(e.shortName(), e);
    
    loader->unload();
  }
  // Write the list of PluginEntrys to disk
  return writePluginIndex();
}


bool PluginManager::writePluginIndex() {
  QSettings conf;
  const QList<PluginEntry> &plugins = _availablePlugins.values();
  
  conf.beginGroup("mClient");
  conf.beginGroup("plugins");
  conf.setValue("path", QVariant(getConfig()->getPluginPath()));
  conf.setValue("indexed", QVariant(QDateTime(QDateTime::currentDateTime())
				    .toString()));
  conf.endGroup(); /* plugins */
  conf.beginWriteArray("plugins");
  for (int i = 0; i < plugins.size(); ++i) {
    const PluginEntry &e = plugins.at(i);
    conf.setArrayIndex(i);
    conf.setValue("shortname", QVariant(e.shortName()));
    conf.setValue("libname", QVariant(e.libName()));
    conf.setValue("longname", QVariant(e.longName()));
    
    if (!e.apiList().isEmpty()) {
      conf.beginWriteArray("api");
      QStringList api = e.apiList();
      for(int j = 0; j < api.size(); ++j) {
	conf.setArrayIndex(j);
	conf.setValue("name", QVariant(api.at(j)));
	conf.setValue("version",
		      QVariant(QString::number(e.version(api.at(j)))));
      }
      conf.endArray(); /* api */
    }
  }
  conf.endArray(); /* plugins */
  conf.endGroup(); /* mClient */
  
  qDebug("* plugin index written");
  return true;
}


bool PluginManager::readPluginIndex() {
    // Read in the plugin db xml into PluginEntrys
  QSettings conf;
    
  conf.beginGroup("mClient");
  conf.beginGroup("plugins");
  QDateTime generated = 
    QDateTime::fromString(conf.value("generated").toString());
  conf.endGroup();
  int pluginsSize = conf.beginReadArray("plugins");
    for (int i = 0; i < pluginsSize; ++i) {
      PluginEntry e;
      conf.setArrayIndex(i+1); /* plugins/index */
      e.libName(conf.value("libname").toString());
      e.longName(conf.value("longname").toString());
      e.shortName(conf.value("shortname").toString());

      int apiSize = conf.beginReadArray("api");
      for (int j = 0; j < apiSize; ++j) {
	conf.setArrayIndex(j+1); /* plugins/index */
	QString name = conf.value("name").toString();
	int version = conf.value("version").toInt();
	e.addAPI(name, version);
      }
      conf.endArray(); /* api */

      // Insert the plugin
      _availablePlugins.insert(e.shortName(), e);
    }
    conf.endArray(); /* plugins */
    conf.endGroup(); /* mClient */

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
