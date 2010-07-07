#ifndef PLUGINSESSION_H
#define PLUGINSESSION_H

#include "AbstractPluginSession.h"

#include <QThread>
#include <QHash>
#include <QMultiMap>
#include "MClientPluginInterface.h"

class PluginManager;
class PluginLoader;
class PluginEntry;

class MClientEventHandler;

class CommandProcessor;
class ActionManager;
class AliasManager;

class ConfigEntry;

class QApplication;
class QEvent;
class QPluginLoader;

typedef QHash<QString, QPluginLoader*> PluginHash;

class PluginSession : public AbstractPluginSession {
  Q_OBJECT

  public:
    PluginSession(const QString&, PluginManager *pm);
    ~PluginSession();

    // For posting events
    void customEvent(QEvent* e);

    PluginManager* getManager() const { return _pluginManager; }
    //CommandProcessor* getCommand() const { return _commandProcessor; }
    QObject* getCommand() const { return (QObject*)_commandProcessor; }
    AliasManager* getAlias() const { return _aliasManager; }
    ActionManager* getAction() const { return _actionManager; }

    const QString& session() const { return _session; }
    const PluginHash loadedPlugins() const { return _loadedPlugins; }

    // For EventHandlers to easily receive the ConfigEntry
    ConfigEntry* retrievePluginSettings(const QString &pluginName) const;

    bool isMUME() const { return _mume; }

public slots:
    void doneLoading();

 signals:
    void sendPlugins(const PluginHash &);
    void doneLoading(PluginSession *);

 protected:
    void run();

    // For starting/stopping the session (accessible by PluginManager)
    void startSession();
    void stopSession();
    
    // Some of these protected functions should only be accessible by the
    // PluginManager which handles whether or not a session should start
    // or stop
    friend class PluginManager;

  private:
    bool _mume;
    QString _session;
    PluginManager *_pluginManager;
    CommandProcessor *_commandProcessor;
    ActionManager *_actionManager;
    AliasManager *_aliasManager;
    
    // For loading the plugins and checking for errors
    void loadAllPlugins();
    bool loadPlugin(const QString &libName);
    bool checkDependencies(MClientPluginInterface *iPlugin);

    const QPluginLoader* pluginWithAPI(const QString &api) const;

    // A hash of the plugin object pointers, short name -> pointer
    // NOTE: this way of indexing plugins gives each one a unique slot
    PluginHash _loadedPlugins;
  
    // A hash of the plugin object pointers, api -> pointer
    // NOTE: some plugins may be repeated here.
    PluginHash _pluginAPIs;
  
    // A hash of the plugin object pointers, data type -> pointer
    // NOTE: this is used to keep track of what data types each plugin
    // wants for easy event handling.  This one also may have multiple
    // entries for a given plugin.
    QHash<QString, QMultiMap<int, MClientEventHandler*> > _receivesTypes;
    QHash<QString, MClientEventHandler*> _receivesType;
};

#endif /* PLUGINSESSION_H */
