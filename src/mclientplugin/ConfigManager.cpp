#include "ConfigManager.h"

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QtXml>


ConfigManager* ConfigManager::_pinstance = 0;

ConfigManager* ConfigManager::instance() {
    if(!_pinstance) {
        _pinstance = new ConfigManager();
    }

    return _pinstance;
}


void ConfigManager::destroy() {
    delete this;
}


bool sortXmlElements(const QString &s1, const QString &s2) {
  QStringList t1 = s1.split("/");
  QStringList t2 = s2.split("/");

  QString e1 = t1.takeLast();
  QString e2 = t2.takeLast();
  QString j1 = t1.join("/");
  QString j2 = t2.join("/");

  if (j2.startsWith(j1)) {
    if (t1.size() == t2.size())
      return s1.toLower() < s2.toLower();
    else
      return t1.size() < t2.size();
  }
  else 
    return j1.toLower() <= j2.toLower();
  
}

bool readXmlFile(QIODevice &device, QSettings::SettingsMap &map) {
  QXmlStreamReader xml(&device);

  QStringList elementStack;

  while (!xml.atEnd()) {
    xml.readNext();

    // If we start an element then add it to the stack
    if(xml.isStartElement()) {
      QString name = xml.name().toString();
      name.replace(QRegExp("^array(\\d+)$"), "\\1");
      elementStack.append(name);

      // See if there are attributes, if there aren't, then the data
      // is held within the text
      if (xml.attributes().isEmpty()) {
	QString key = elementStack.join("/");
	QVariant value = QVariant(xml.readElementText());
	if (!value.toString().trimmed().isEmpty())
	  map.insert(key, value);
	
      } else {
	// The data is usually held within the attributes
	foreach(QXmlStreamAttribute a, xml.attributes()) {
	  QString key = elementStack.join("/") + "/" + a.name().toString();
	  QVariant value = QVariant(a.value().toString());
	  map.insert(key, value);
	}
      }

    }
    // If we end an element, then pop it from the stack
    else if (xml.isEndElement()) {
      elementStack.removeLast();

    }
    // Otherwise we have encountered an error
    else if (xml.hasError())
      qWarning() << "* Error parsing XML configuration file"
		 << xml.errorString();
  }
  qDebug() << "* read in XML file"; // << map;
}

bool writeXmlFile(QIODevice &device, const QSettings::SettingsMap &map) {
  QXmlStreamWriter xml(&device);

  // A list of available keys sorted from smallest to largest in size
  // as well as alphabetically
  QStringList keys = map.keys();
  qSort(keys.begin(), keys.end(), sortXmlElements);

  // The empty stack
  QStringList elementStack;

  xml.setAutoFormatting(true);
  xml.writeStartDocument();
  for (int i = 0; i < keys.size(); ++i) {
    // Figure out where we are in the DOM
    QStringList currentElements = keys.at(i).split("/");
    currentElements.replaceInStrings(QRegExp("^(\\d+)$"), "array\\1");
    QString currentAttribute = currentElements.takeLast();
   
    // If we have moved up the tree, add some elements
    if (elementStack.size() < currentElements.size()) {
      for (int j = elementStack.size(); j < currentElements.size(); ++j) {
	elementStack.append(currentElements.at(j));
	xml.writeStartElement(elementStack.last());
      }
    }
    // If we have moved down the tree, remove some elements, check and
    // make sure that we are still on the same element, if not, create it
    else if (elementStack.size() > currentElements.size()) {
      for (int j = currentElements.size(); j <= elementStack.size(); ++j) {
	elementStack.removeLast();
	xml.writeEndElement();
      }
    }

    // Check if the current element does indeed match, if not, update
    // the element to match
    if (elementStack.size() == currentElements.size() &&
	elementStack.size() > 0) {
      if (elementStack.last() != currentElements.last()) {
	xml.writeEndElement();
	elementStack.removeLast();
	elementStack.append(currentElements.last());
	xml.writeStartElement(elementStack.last());
      }
    }

    //qDebug() << "stack: " << elementStack << currentAttribute;
    //qDebug() << "stream:" << currentElements << currentAttribute;

    // Write the value usually as an attribute of an element
    if (elementStack.size() > 0)
      xml.writeAttribute(currentAttribute, map[keys.at(i)].toString());
    // if we can't, then write it as text element
    else
      xml.writeTextElement(currentAttribute, map[keys.at(i)].toString());

  }
  xml.writeEndDocument();

  qDebug() << "* wrote out XML file"; // << map;
}

const QSettings::Format XmlFormat = 
QSettings::registerFormat("xml", readXmlFile, writeXmlFile);


ConfigManager::ConfigManager(QObject* parent) : QObject(parent) {
    _appSettings = 0;
    readApplicationSettings();
    qDebug() << "ConfigManager created with thread:" << this->thread();
}


ConfigManager::~ConfigManager() {
}


const bool ConfigManager::readApplicationSettings() {
  QDir configDir = QDir(qApp->applicationDirPath());
  configDir.cdUp();
  if (!configDir.exists("config")) {
    if (!configDir.mkdir("config")) {
      qCritical() << "* Unable to create configuration directory";
    }
    qDebug() << "* created configuration directory" << configDir.path();
  }
  
  QSettings conf("config/mClient.xml", XmlFormat);
  //QSettings conf(XmlFormat, QSettings::UserScope, "MUME", "mClient");
  //conf.setPath(XmlFormat, QSettings::UserScope, "./config");

  conf.beginGroup("mClient");
  conf.beginGroup("general");
  _configPath = conf.value("config_path", "./config").toString();
  conf.endGroup(); /* general */
  conf.endGroup(); /* mClient */

  // Transfer results to settings hash
  _appSettings = new QHash<QString, QString>;
  foreach(QString s, conf.allKeys())
    _appSettings->insert(s, conf.value(s).toString());

  qDebug() << "* read in application settings";
  return true;
}


const bool ConfigManager::writeApplicationSettings() {
  QSettings conf("config/mClient.xml", XmlFormat);

  _appSettings->insert("mClient/version", "1.0");
  _appSettings->insert("mClient/general/config_path", _configPath);
  
  // Place the hash values back into the settings
  QHash<QString, QString>::const_iterator i = _appSettings->constBegin();
  while (i != _appSettings->constEnd()) {
    conf.setValue(i.key(), i.value());
    ++i;
  }
  
  qDebug() << "* wrote out application settings";
  return true;
}


const bool ConfigManager::readPluginSettings(const QString &pluginName) {
  QString file = QString("%1/%2.xml").arg(_configPath, pluginName);
  QSettings conf(file, XmlFormat);

  // Transfer the results to the settings hash
  QHash<QString, QString> *hash = new QHash<QString, QString>;
  foreach(QString s, conf.allKeys())
    hash->insert(s, conf.value(s).toString());
  _pluginSettings.insert(pluginName, hash);

  // Identify the profiles within this plugin
  if (conf.contains("config/profile/name")) {
    QString profile = conf.value("config/profile/name").toString();
    qDebug() << "* found profile" << profile << "for" << pluginName;
    _profiles.insert(profile, pluginName);
  }

  qDebug() << "* read config file" << file;
  return true;
}

const bool ConfigManager::writePluginSettings(const QString &pluginName) {
  QString file = QString("%1/%2.xml").arg(_configPath, pluginName);
  QSettings conf(file, XmlFormat);

  // Place the hash values back into the settings
  QHash<QString, QString>::const_iterator i =
    _pluginSettings[pluginName]->constBegin();
  while (i != _pluginSettings[pluginName]->constEnd()) {
    conf.setValue(i.key(), i.value());
    ++i;
  }
  
  qDebug() << "* wrote config file" << file;
  return true;
}
