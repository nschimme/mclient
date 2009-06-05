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


bool readXmlFile(QIODevice &device, QSettings::SettingsMap &map) {
  QXmlStreamReader xml(&device);

  QStringList elementStack;

  while (!xml.atEnd()) {
    xml.readNext();
    if(xml.isStartElement()) {
      elementStack.append(xml.name().toString());
      foreach(QXmlStreamAttribute a, xml.attributes()) {
	QString key = elementStack.join("/") + "/" + a.name().toString();
	QVariant value = QVariant(a.value().toString());
	map.insert(key, value);
      }
    }
    else if (xml.isEndElement())
      elementStack.removeLast();
    else if (xml.hasError())
      qWarning() << "* Error parsing XML configuration file";
  }
  qDebug() << "* read in XML file" << map;
}

bool writeXmlFile(QIODevice &device, const QSettings::SettingsMap &map) {
  QXmlStreamWriter xml(&device);
  
  QHash<QString, QVariant> h;

  QMap<QString, QVariant>::const_iterator i = map.constBegin();
  while (i != map.constEnd()) {
    QStringList parts = i.key().split("/");
    ++i;
  }

  //QList<QList<QString> > list;

  // xml.setAutoFormatting(true);
//   xml.writeStartDocument();
//   xml.writeStartElement(parts.takeFirst());
//   xml.writeAttribute(parts.join("/"), i.value().toString());
//   xml.writeEndElement();
//   xml.writeEndDocument();

  qDebug() << "* wrote out XML file" << map;
}

const QSettings::Format XmlFormat = 
QSettings::registerFormat("xml", readXmlFile, writeXmlFile);


ConfigManager::ConfigManager(QObject* parent) : QObject(parent) {
    readApplicationSettings();
    readPluginSettings();
    qDebug() << "ConfigManager created with thread:" << this->thread();
}


ConfigManager::~ConfigManager() {
}


const bool ConfigManager::readApplicationSettings() {
  QSettings conf("config/mClient.xml", XmlFormat);

//   QSettings conf(XmlFormat, QSettings::UserScope, "MUME", "mClient");
//   conf.setPath(XmlFormat, QSettings::UserScope, "./config");

  conf.beginGroup("General");
  _configPath = conf.value("config_path", "./config").toString();
  conf.endGroup();

  //  QSettings test("config/socketmanagerio.xml", XmlFormat);

  qDebug() << "* read in application settings";
  return true;
}


const bool ConfigManager::writeApplicationSettings() const {
  QSettings conf("config/mClient.xml", XmlFormat);

  conf.beginGroup("General");
  conf.setValue("config_path", _configPath);
  conf.endGroup();

  qDebug() << "* wrote out application settings";
  return true;
}


const bool ConfigManager::readPluginSettings() {
    QDir d(_configPath);
    QStringList filters;
    filters << "*.xml";
    d.setNameFilters(filters);
    foreach(QString fn, d.entryList(QDir::Files)) {
        qDebug() << "* examining config file" << fn;
        QIODevice* device = new QFile(_configPath+"/"+fn);
        if(!device->open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open file for reading!" << fn;
            continue;
        }

        QXmlStreamReader* xml = new QXmlStreamReader(device);

        QString plugin;
        QString profile;
        QString id;
        while(!xml->atEnd()) {
            xml->readNext();

            if(xml->isEndElement()) {
                if(xml->name() == "config") {
                } else if(xml->name() == "profile") {
                }
            
            } else if(xml->isStartElement()) {
                if(xml->name() == "config") {
                    // check version, get plugin name
                    QXmlStreamAttributes attr = xml->attributes();
                    QString version = attr.value("version").toString();
                    plugin = attr.value("plugin").toString();

                    QHash<QString,
                        QHash<QString,
                            QHash<QString, QString>
                        >
                    > h;

                    _config.insert(plugin, h);

                } else if(xml->name() == "profile") {
                    QXmlStreamAttributes attr = xml->attributes();
                    profile = attr.value("name").toString();
                    
                    QHash<QString,
                        QHash<QString, QString>
                    > h;

                    _config[plugin].insert(profile, h);
                    _profiles[profile] << plugin;
                
                } else {
                    QString tag = xml->name().toString();
                    QXmlStreamAttributes attr = xml->attributes();
                    //id = attr.value("id").toString();
		    id = tag; // Why use an id and not the tag?

                    if(!_config[plugin][profile].contains(id)) {
		        QHash<QString, QString> h;
                        _config[plugin][profile].insert(id, h);
                    }
                    
                    foreach(QXmlStreamAttribute a, attr) {
                        QString key = a.name().toString();
                        QString value = a.value().toString();
                        _config[plugin][profile][id].insert(key, value);
                    }
                }
            }
        }
    }

    qDebug() << "* config looks like:" << _config;
    qDebug() << "* profiles look like:" << _profiles;

    return true;
}


const bool ConfigManager::writePluginSettings() const {
    return true;
}


const QHash<QString, QHash<QString, QString> > 
ConfigManager::pluginProfileConfig(const QString plugin, 
        const QString profile) const {
    return _config[plugin][profile];
}
