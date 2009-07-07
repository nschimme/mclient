#include "MClientPlugin.h"

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

#include <QPluginLoader>

MClientPlugin::MClientPlugin(QObject* parent) : QThread(parent) {
    _shortName = "mclientplugin";
    _longName = "The Original MClientPlugin";
    _description = "If you see this text, the plugin author did not replace the default description.";
    _configurable = false;
    _configVersion = "none";
}


MClientPlugin::~MClientPlugin() {
}


const QString& MClientPlugin::shortName() const {
    return _shortName;
}


const QString& MClientPlugin::longName() const {
    return _longName;
}


const QString& MClientPlugin::description() const {
    return _description;
}


const QString& MClientPlugin::version() const {
    return _version;
}


const QHash<QString, int> MClientPlugin::implemented() const {
    return _implemented;
}


const QHash<QString, int> MClientPlugin::dependencies() const {
    return _dependencies;
}


const QStringList& MClientPlugin::receivesDataTypes() const {
    return _receivesDataTypes;
}


const QStringList& MClientPlugin::deliversDataTypes() const {
    return _deliversDataTypes;
}


bool MClientPlugin::configurable() const {
    return _configurable;
}


#if QT_VERSION < 0x0040400
// This is needed in Qt 4.3 because run is pure virtual -- not in 4.4
void MClientPlugin::run() {
}
#endif

// Post an event
void MClientPlugin::postSession(QVariant* payload, QStringList tags) {
  bool found = false;
  foreach (QString s, tags) {
    // Iterate through all the data types
    //qDebug() << "* finding data type" << s << "out of" << me->dataTypes();
    
    QMultiHash<QString, QObject*>::iterator it
      = _receivesTypes.find(s);
    while (it != _receivesTypes.end() && it.key() == s) {
      
      qDebug() << "# posting" << tags << "to" 
	       << it.value() << "with" << payload;
      
      // Post the event
      MClientEventData *med = new MClientEventData(payload, tags, _session);
      MClientEvent* me = new MClientEvent(med);
      QCoreApplication::postEvent(it.value(), me);
      found = true;
      
      ++it; // Iterate
    }
  }
  if (!found) {
    qWarning() << "! No plugins accepted data types" << tags;

    qWarning() << "! Rerouting event to session";
    MClientEventData *med = new MClientEventData(payload, tags, _session);
    MClientEvent* me = new MClientEvent(med);
    QCoreApplication::postEvent(_pluginSession, me);

  }
}


void MClientPlugin::postManager(QVariant* payload, QStringList tags, 
			      QString session) {
  MClientEvent* me = new MClientEvent(new MClientEventData(payload, tags,
							   session));
  
  QApplication::postEvent(_pluginSession->getManager(), me);
}


// Receive the PluginSession reference upon load
void MClientPlugin::setPluginSession(PluginSession *ps) {
  _pluginSession = ps;
  _session = ps->session();
}


// Handles MClientEngineEvent
void MClientPlugin::engineEvent(QEvent *e) {
  MClientEngineEvent* ee = static_cast<MClientEngineEvent*>(e);
  qDebug() << "#" << _shortName << "got engineEvent" << ee->dataType();
  if (ee->dataType() == EE_MANAGER_POST) {
    // We should never get one of these
    
  }
  else if (ee->dataType() == EE_DATATYPE_UPDATE) {
    // Contains a hash of receiving plugins we need to post to
    QHash<QString, QVariant> raw = ee->payload()->toHash();
    //qDebug() << raw;
    
    QHash<QString, QObject*> hash;
    QHash<QString, QVariant>::const_iterator i;
    for (i = raw.constBegin(); i != raw.constEnd(); ++i)
      hash.insertMulti(i.key(), i.value().value<QObject*>() );
    
    _receivesTypes.unite(hash);

  }
}

