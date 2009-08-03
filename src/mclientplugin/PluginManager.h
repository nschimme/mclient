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
        static PluginManager* instance();
        ~PluginManager();

	MainWindow* getMainWindow() { return _mainWindow; }
	ConfigManager* getConfig() { return _configManager; }
	PluginSession* getPluginSession(const QString &s) {
	  return _pluginSessions[s];
	}
	
	// For PluginSession
	QHash<QString, PluginEntry*> getAvailablePlugins() {
	  return _availablePlugins;
	}

        void customEvent(QEvent* e);
        void configure();

public slots:
        void initSession(const QString &s);
        void stopSession(const QString &s);
        void initDisplay(PluginSession *ps);

    protected:
        PluginManager(QObject *parent=0);
        static PluginManager* _pinstance;

    private:
        bool indexPlugins();
        bool writePluginIndex();
        bool readPluginIndex();

	MainWindow *_mainWindow;
	ConfigManager *_configManager;

	// Sessions
	QHash<QString, PluginSession*> _pluginSessions;
        
        // short name -> other info
        QHash<QString, PluginEntry*> _availablePlugins; 

        QPointer<PluginConfigWidget> _configWidget;

 signals:
	void doneLoading();

};


#endif // #PLUGINMANAGER_H
