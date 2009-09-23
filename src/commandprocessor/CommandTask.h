#ifndef COMMANDTASK_H
#define COMMANDTASK_H

#include <QObject>
#include <QStringList>
#include <QSemaphore>

#include "CommandEntry.h"

class CommandProcessor;
class AliasManager;
class ActionManager;

class CommandTask : public QObject {
  Q_OBJECT
    
    public:
        CommandTask(CommandProcessor *);
        ~CommandTask();

public slots:
	void parseMudOutput(const QString &, const QStringList &);
        void parseUserInput(const QString &);
        void socketOpen(bool);

 protected:
	QSemaphore _semaphore;

   private:
	QStringList _queue;
	uint _stack;
	CommandProcessor *_commandProcessor;
	AliasManager *_aliasManager;
	ActionManager *_actionManager;
	bool _verbose;
	bool _socketOpen;

	void displayData(const QString &);
	void displayPrompt(const QString &);
	void postSession(QVariant *, const QStringList &);

	bool processStack();
	
        bool findCommand(const QString &cmd, QString &arg);
	bool internalCommand(const QString &cmd, QString &arg);
	const QString& parseArguments(QString &,
				      CommandEntryType =CMD_ONE_LINE);

	bool findAlias(const QString &, QString &);
	bool findAction(const QString &, QStringList);

	bool handleAliasCommand(const QString &);
	bool handleActionCommand(const QString &);

	friend class CommandProcessor;

};


#endif /* COMMANDTASK_H */
