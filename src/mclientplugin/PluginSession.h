#ifndef PLUGINSESSION_H
#define PLUGINSESSION_H

#include <QThread>
#include <QHash>
#include <QMultiHash>
#include "MClientPluginInterface.h"

class PluginManager;
class PluginLoader;
class PluginEntry;

class QApplication;
class QEvent;
class QPluginLoader;

class PluginSession : public QThread {
  Q_OBJECT
    
  public:
    PluginSession(const QString&, PluginManager *pm, QObject *parent=0);
    ~PluginSession();

    // For posting events
    void customEvent(QEvent* e);

    // For starting/stopping the session
    void initDisplay();
    void startSession();
    void stopSession();

    PluginManager* getManager() { return _pluginManager; }
    const QString& session() { return _session; }

 protected:
        void run();

  private:
    QString _session;
    PluginManager *_pluginManager;
    
    // For loading the plugins and checking for errors
    void loadAllPlugins();
    bool loadPlugin(const QString &libName);
    bool checkDependencies(MClientPluginInterface *iPlugin);

    // For posting datatypes to each plugin
    void postReceivingPlugins();

    const QPluginLoader* pluginWithAPI(const QString &api) const;

    // A hash of the plugin object pointers, short name -> pointer
    // NOTE: this way of indexing plugins gives each one a unique slot
    QHash<QString, QPluginLoader*> _loadedPlugins;
  
    // A hash of the plugin object pointers, api -> pointer
    // NOTE: some plugins may be repeated here.
    QHash<QString, QPluginLoader*> _pluginAPIs;
  
    // A hash of the plugin object pointers, data type -> pointer
    // NOTE: this is used to keep track of what data types each plugin
    // wants for easy event handling.  This one also may have multiple
    // entries for a given plugin.
    QMultiHash<QString, QPluginLoader*> _receivesTypes;
    
    // A hash of plugin object pointers, pointer -> data type
    QMultiHash<QPluginLoader*, QString> _deliversTypes;

 signals:
    void sendWidgets(const QList< QPair<int, QWidget*> >&);
    void doneLoading(PluginSession*);
};

#endif /* PLUGINSESSION_H */
