#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <QThread>
#include <QMap>
#include <QMultiHash>
#include <QStringList>
#include <QEvent>

class CommandEntry;
class CommandTask;
class PluginSession;
class MClientCommandHandler;

typedef QMap<QString, CommandEntry*> CommandMapping;
typedef QHash<QString, MClientCommandHandler*> CommandHandlerHash;

class CommandProcessor : public QThread {
    Q_OBJECT
    
    public:
        CommandProcessor(PluginSession *);
        ~CommandProcessor();

	// For receiving events
	void customEvent(QEvent* e);

	bool unregisterCommand(const QString &source);
	void registerCommand(const QString &,
			     const QList<CommandEntry *> &);

	CommandMapping getCommandMapping() { return _mapping; }
	CommandHandlerHash getHandlers() { return _handlers; }
	QChar getCommandSymbol() { return _symbol; }
	QChar getDelimSymbol() { return _delim; }
	PluginSession* getPluginSession() { return _pluginSession; }
	void emitQuit();

	CommandTask* getTask() const { return _task; }

public slots:
        void initHandlers();

 protected:
	void run();

   private:
	/** Commands Section */
        QChar _symbol, _delim;
	CommandMapping _mapping;
	CommandHandlerHash _handlers;

	PluginSession *_pluginSession;
	CommandTask *_task;

 signals:
	void quit();
	void parseUserInput(const QString&);
	void parseMudOutput(const QString &, const QStringList &);
	void socketOpen(bool);

};


#endif /* COMMANDPROCESSOR_H */
