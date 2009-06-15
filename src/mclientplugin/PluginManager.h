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

class CommandManager;
class ConfigManager;
class MainWindow;

class PluginManager : public QObject {
    Q_OBJECT

    public:
        static PluginManager* instance();
        ~PluginManager();

	MainWindow* getMainWindow() { return _mainWindow; }
	CommandManager* getCommand() { return _commandManager; }
	ConfigManager* getConfig() { return _configManager; }
	PluginSession* getPluginSession(const QString &s) {
	  return _pluginSessions[s];
	}
	
	// For PluginSession
	QHash<QString, PluginEntry*> getAvailablePlugins() {
	  return _availablePlugins;
	}
	QString getPluginDir() { return _pluginDir; }

        void customEvent(QEvent* e);
        void configure();

public slots:
        void initSession(const QString &s);
        void stopSession(const QString &s);
        void startSession(PluginSession *ps);

    protected:
        PluginManager(QObject *parent=0);
        static PluginManager* _pinstance;

    private:
        bool indexPlugins();
        bool writePluginIndex();
        bool readPluginIndex();

	MainWindow *_mainWindow;
	CommandManager *_commandManager;
	ConfigManager *_configManager;

	// Sessions
	QHash<QString, PluginSession*> _pluginSessions;
        
        // short name -> other info
        QHash<QString, PluginEntry*> _availablePlugins; 

        // In what directory are plugins located?  ConfigManager knows.
        QString _pluginDir;

        // The plugin index
        QString _pluginIndex;

        QPointer<PluginConfigWidget> _configWidget;

 signals:
	void doneLoading();

};


#endif // #PLUGINMANAGER_H
