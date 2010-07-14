#ifndef ABSTRACTPLUGINSESSION_H
#define ABSTRACTPLUGINSESSION_H

#include <QThread>
#include <QString>

class ConfigEntry;

class AbstractPluginSession : public QThread {
  Q_OBJECT
    
    public:
        AbstractPluginSession(QObject *parent=0) : QThread(parent) {}
  
        // The short name of the plugin used in hashes and maps
        virtual const QString& session() const=0;

	// Grab the event handler
	virtual ConfigEntry* retrievePluginSettings(const QString &pluginName) const=0;

	// Is MUME?
	virtual bool isMUME() const=0;

	// CommandProcessor
	virtual QObject* getCommand() const=0;
};

#endif // ABSTRACTPLUGINSESSION_H
