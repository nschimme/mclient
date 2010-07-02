#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QHash>
#include <QMultiHash>
#include <QPointer>

class PluginConfigWidget;
class PluginEntry;
class PluginSession;

class QApplication;
class QEvent;
class QPluginLoader;
class QString;
class QWidget;

class ConfigManager;
class MainWindow;

class PluginManager : public QObject {
  Q_OBJECT

    public:
        static PluginManager* getInstance();
        ~PluginManager();

	MainWindow* getMainWindow() const { return _mainWindow; }
	ConfigManager* getConfig() const { return _configManager; }
	PluginSession* getPluginSession(const QString &s) const {
	  return _pluginSessions[s];
	}
	
	// For PluginSession
	const QHash<QString, PluginEntry>& getAvailablePlugins() const {
	  return _availablePlugins;
	}

        void customEvent(QEvent* e);
        void configure();

public slots:
        void initSession(const QString &s);
        void stopSession(const QString &s);

 signals:
	// We emit this signal after having found the available plugins
	void doneLoading();
	
 protected:
        PluginManager(QObject *parent=0);

 private:
        static PluginManager* _instance;
	PluginManager& operator=(PluginManager const&);

        bool indexPlugins();
        bool writePluginIndex();
        bool readPluginIndex();

	MainWindow *_mainWindow;
	ConfigManager *_configManager;

	// Sessions
	QHash<QString, PluginSession*> _pluginSessions;
        
        // short name -> other info
        QHash<QString, PluginEntry> _availablePlugins; 

        QPointer<PluginConfigWidget> _configWidget;

};


#endif // #PLUGINMANAGER_H
