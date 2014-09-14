#include "ConfigManager.h"

#include "ConfigEntry.h"

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QtXml>

class ConfigModel : public QAbstractTableModel {
  public:
  ConfigModel (ConfigManager *manager)
    : QAbstractTableModel (0), _mgr(manager) {}
  
  virtual ~ConfigModel() {}

  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const {
    if (parent.isValid()) return 0;  // because Qt docs say so
    return 1;  // we have 1 column
  }
  
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const {
    if (parent.isValid()) return 0;  // because Qt docs say so
    return _mgr->profileNames().size();
  }
  
  virtual QVariant headerData(int section, Qt::Orientation /*orientation*/,
			      int role = Qt::DisplayRole) const {
    if (role != Qt::DisplayRole) return QVariant();
    switch (section) {
    case 0: return QVariant("Profile");
    default: return QVariant();
    };
  }

  virtual QVariant data(const QModelIndex &index,
			int role = Qt::DisplayRole) const {
    // display role only
    if (role != Qt::DisplayRole) return QVariant();
    
    if (index.parent().isValid()) return QVariant();
    int row = index.row();
    int col = index.column();
    // Return something else if we're out of bounds
    if ((col < 0) || (col > columnCount())) return QVariant();
    
    if ((row < 0) || (row > _mgr->profileNames().size())) return QVariant();
    // fetch the requested information and return it
    switch (col) {
    case 0: return QVariant(_mgr->profileNames()[row]);
    default: return QVariant();
    };
  }

  void rowChanged(int row) {
    emit dataChanged(index(row, 0), index(row, 3));
  }

  void addRow(int row) {
    beginInsertRows(QModelIndex(), row, row);
  }

  void rowAdded() {
    endInsertRows();
  }

  void removeRow(int row) {
    beginRemoveRows(QModelIndex(), row, row);
  }

  void rowRemoved() {
    endRemoveRows();
  }

  private:
  ConfigManager *_mgr;
};


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
    //qDebug() << elementStack << xml.tokenString();
    xml.readNext();

    // If we start an element then add it to the stack
    if (xml.isStartElement()) {
      QString name = xml.name().toString();
      name.replace(QRegExp("^array(\\d+)$"), "\\1");
      elementStack.append(name);
	
      // Read attributes
      if (!xml.attributes().isEmpty()) {
	foreach(QXmlStreamAttribute a, xml.attributes()) {
	  QString key = elementStack.join("/") + "/" + a.name().toString();
	  QVariant value = QVariant(a.value().toString());
	  map.insert(key, value);
	  //qDebug() << "attribute" << key << value;
	}
      }
      
    }
    // If we end an element, then pop it from the stack
    else if (xml.isEndElement()) {
      QString name = xml.name().toString();
      name.replace(QRegExp("^array(\\d+)$"), "\\1");
      if (name != elementStack.last())
	qWarning() << "! Error, this endElement was" << name
		   << ", not" << elementStack.last();
      elementStack.removeLast();
      
    }
    // Are these characters?
    else if (xml.isCharacters()) {
      QString key = elementStack.join("/");
      QVariant value = QVariant(xml.text().toString());
      if (!value.toString().trimmed().isEmpty()) {
	map.insert(key, value);
	//qDebug() << "characters" << key << value;
      }
      
    }
    // Otherwise we have encountered an error
    else if (xml.hasError()) {
      qWarning() << "! Error parsing XML configuration file"
		 << xml.errorString();
      return false;
    }
  }

  qDebug() << "* read in XML file"; // << map;
  return true;
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
    if (elementStack.size() == 0)
      xml.writeTextElement(currentAttribute, map[keys.at(i)].toString());
    else {
      QString data = map[keys.at(i)].toString();
      if (!data.contains("\n"))
	xml.writeAttribute(currentAttribute, data);
      else
	xml.writeTextElement(currentAttribute, data);
    }
  }
  xml.writeEndDocument();

  qDebug() << "* wrote out XML file"; // << map;
  return true;
}

const QSettings::Format
XmlFormat = QSettings::registerFormat("xml", readXmlFile, writeXmlFile);


ConfigManager::ConfigManager(QObject* parent) : QObject(parent) {
    _model = new ConfigModel(this);
    _appSettings = 0;
    readApplicationSettings();
    qDebug() << "ConfigManager created with thread:" << this->thread();
}


ConfigManager::~ConfigManager() {
  delete _model;
  qDebug() << "* ConfigManager destroyed";
}


QAbstractTableModel *ConfigManager::model () const {
  return _model;
}

bool ConfigManager::readApplicationSettings() {
  QSettings conf("config/mClient.xml", XmlFormat);

  _configPath = conf.value("mClient/config/path", "config").toString();
  _pluginPath = conf.value("mClient/plugins/path", "plugins").toString();

  // Transfer results to settings hash
  _appSettings = new SettingsHash;
  foreach(QString s, conf.allKeys())
    _appSettings->insert(s, conf.value(s));

  qDebug() << "* read in application settings";
  return true;
}


bool ConfigManager::writeApplicationSettings() {
  QSettings conf;

  _appSettings->insert("version", "1.0");
  //_appSettings->insert("config/path", _configPath);
  
  // Place the hash values back into the settings
  SettingsHash::const_iterator i = _appSettings->constBegin();
  while (i != _appSettings->constEnd()) {
    conf.setValue(i.key(), i.value());
    ++i;
  }

  // Add each profile to the index
  conf.beginGroup("mClient");
  conf.beginWriteArray("profiles");
  int j = 0;
  QHash<QString, ConfigEntry*>::iterator k;
  for (k = _profileSettings.begin(); k != _profileSettings.end(); ++k) {
    conf.setArrayIndex(j++);
    conf.setValue("name", k.key());
  }
  conf.endArray();
  conf.endGroup();
  
  qDebug() << "* wrote out application settings";
  return true;
}


bool ConfigManager::discoverProfiles() {
  // Move into the config directory
  QDir configDir = QDir(qApp->applicationDirPath());
  configDir.cd(_configPath);

  // Look at each directory in the config path, as this might be a
  // potential profile
  foreach(QString dirName, configDir.entryList(QDir::AllDirs)) {

    QString absolutePath = configDir.absolutePath() + "/" + dirName;
    qDebug() << absolutePath;

    // A profile directory must contain "profile.xml"
    QFileInfo profileFile(absolutePath, "profile.xml");
    profileFile.setCaching(false);
    if (profileFile.exists()) {
      readProfileSettings(dirName);
      
    } else {
      qDebug() << "! Invalid profile discovered" << dirName;
    }
  }

  // If there are no profiles, add the "Default" one
  if (_profileSettings.isEmpty()) {
    _profileSettings["Default"] = new ConfigEntry("Default");

    qDebug() << "! No profiles found, creating default profile.";

  }

  qDebug() << "* Discovered following profiles:" << profileNames();
  return true;
}


bool ConfigManager::readProfileSettings(const QString &dirName) {
  // Read the plugin settings from this profile directory
  QString file = QString("%1/%2/profile.xml").arg(_configPath, dirName);
  QSettings conf(file, XmlFormat);

  QString profileName = conf.value("profile/name", dirName).toString();

  // Transfer the results to the settings hash
  ConfigEntry *ce = new ConfigEntry(profileName);
  foreach(QString s, conf.allKeys())
    ce->insert(s, conf.value(s));

  // Always update the path to the current directory
  ce->insert("profile/path", dirName);

  // Make sure the ConfigEntry isn't modified (as we just read in the
  // latest settings)
  ce->_modified = false;

  // Add the ConfigEntry to profileSettings
  _profileSettings.insert(profileName, ce);

  qDebug() << "* read in profile" << profileName << "settings";
  return true;
}


bool ConfigManager::writeProfileSettings(const QString &profileName) {
  // Get the hash
  ConfigEntry *ce = _profileSettings[profileName];
  SettingsHash *hash = ce->hash();  

  // Figure out which directory we are writing to
  QString dirName = hash->value("profile/path", profileName).toString();

  QString file = QString("%1/%2/profile.xml").arg(_configPath, dirName);
  QSettings conf(file, XmlFormat);
  
  hash->insert("profile/version", "1.0");
  hash->insert("profile/path", dirName);
  
  // Place the hash values back into the settings
  SettingsHash::const_iterator i = hash->constBegin();
  while (i != hash->constEnd()) {
    conf.setValue(i.key(), i.value());
    ++i;
  }
  
  qDebug() << "* wrote out profile" << profileName << "settings" << hash->keys();
  return true;
}


bool ConfigManager::readPluginSettings(const QString &profileName,
				       const QString &pluginName) {
  qDebug() << "* reading plugin settings" << pluginName << "for profile" << profileName;
  // Figure out which directory we are writing to
  QString dirName
    = _profileSettings[profileName]->value("profile/path",
					   profileName).toString();
  
  // Read the plugin settings from this profile directory
  QString file = QString("%1/%2/%3.xml").arg(_configPath, dirName, pluginName);
  QSettings conf(file, XmlFormat);
  
  // Transfer the results to the ConfigEntry settings hash
  ConfigEntry *ce = new ConfigEntry(pluginName);
  foreach(QString s, conf.allKeys())
    ce->insert(s, conf.value(s));

  // See if the profile hash exists
  QHash<QString, ConfigEntry* > *profileHash;
  if (_pluginSettings.contains(profileName)) {
    profileHash = _pluginSettings[profileName];
  }
  else {
    profileHash = new QHash<QString, ConfigEntry* >;
    _pluginSettings.insert(profileName, profileHash);
  }

  // Transfer the ConfigEntry to the profile hash
  profileHash->insert(pluginName, ce);
    
  qDebug() << "* read plugin config file" << file;
  writePluginSettings(profileName, pluginName);
  return true;
}


bool ConfigManager::writePluginSettings(const QString &profileName,
					const QString &pluginName) {
  // Figure out which directory we are writing to
  QString dirName
    = _profileSettings[profileName]->value("profile/path",
					   profileName).toString();
  
  // Write to the plugin settings in this profile directory
  QString file = QString("%1/%2/%3.xml").arg(_configPath, dirName, pluginName);
  QSettings conf;

  // Get the hash
  ConfigEntry *ce = _pluginSettings[profileName]->value(pluginName);
  SettingsHash *hash = ce->hash();

  // Place the hash values back into the settings
  SettingsHash::const_iterator i = hash->constBegin();
  while (i != hash->constEnd()) {
    conf.setValue("profile/" + dirName + "/" + i.key(), i.value());
    ++i;
  }
  
  qDebug() << "* wrote plugin config file" << file;
  return true;
}


bool ConfigManager::duplicateProfile(const QString &oldProfile,
				     const QString &newProfile) {
  if (!_profileSettings.contains(oldProfile)) {
    qDebug() << "Unable to duplicate profile" << oldProfile
	     << "because it does not exist";
    return false;
    
  }
  else if (_profileSettings.contains(newProfile)) {
    qDebug() << "Unable to duplicate profile" << oldProfile
	     << "because" << newProfile << "already exists";
    return false;
  }

//   QString oldProfilePrefix = QString("config/%1/").arg(oldProfile);
//   QString newProfilePrefix = QString("config/%1/").arg(newProfile);

//   // Go through each plugin within the profile
//   foreach(QString pluginName, profilePlugins(oldProfile)) {

//     // Grab the current plugin's settings hash
//     QHash<QString, QString> *hash = _pluginSettings[oldProfile];

//     // Iterate through each key, looking for a certain profile
//     QHash<QString, QString>::iterator i;
//     for (i = hash->begin(); i != hash->end(); ++i) {
//       if (i.key().startsWith(oldProfilePrefix)) {
	
// 	// Replace the name, and insert into the hash
// 	QString key = i.key();
// 	key.replace(0, oldProfilePrefix.size(), newProfilePrefix);
// 	hash->insert(key, i.value());
	
//       }
//     }
    
//   }
  return true;
}


bool ConfigManager::deleteProfile(const QString &/*profile*/) {
//   if (!_profilePlugins.contains(profile)) {
//     qDebug() << "Unable to delete profile" << profile
// 	     << "because it does not exist";
//     return false;
    
//   }

//   QString profilePrefix = QString("config/%1/").arg(profile);

//   // Go through each plugin within the profile
//   foreach(QString pluginName, profilePlugins(profile)) {
    
//     // Grab the current plugin's settings hash
//     QHash<QString, QString> *hash = _pluginSettings[pluginName];

//     // Iterate through each key, looking for a certain profile
//     QHash<QString, QString>::iterator i;
//     for (i = hash->begin(); i != hash->end(); ++i) {

//       // Remove keys that are within this profile
//       if (i.key().startsWith(profilePrefix))
// 	hash->remove(i.key());
      
//     }
//   }
  return true;
}
