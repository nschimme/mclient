#ifndef ABSTRACTCOMMANDTASK_H
#define ABSTRACTCOMMANDTASK_H

#include <QThread>
#include <QStringList>

#include "CommandEntry.h"

class CommandProcessor;
class AliasManager;
class ActionManager;

class AbstractCommandTask : public QThread {
    
    public:
        AbstractCommandTask(CommandProcessor*, QObject *parent=0);
        ~AbstractCommandTask();

	virtual void customEvent(QEvent *e);

 protected:
	void run();
	void parseMudOutput(const QString &, const QStringList &);
        void parseUserInput(const QString &);

	bool _verbose;
	bool _socketOpen;

   private:
	QStringList _queue;
	uint _stack;
	CommandProcessor *_commandProcessor;
	AliasManager *_aliasManager;
	ActionManager *_actionManager;

	bool processStack();
	
        bool findCommand(const QString &, QString &);
	bool findAlias(const QString &, QString &);

	const QString& parseArguments(QString &,
				      CommandEntryType =CMD_ONE_LINE);

	bool findAction(const QString &, QStringList);

	bool handleAliasCommand(const QString &);
	bool handleActionCommand(const QString &);

	void displayData(const QString &);
	void postSession(QVariant *, const QStringList &);
};


#endif /* ABSTRACTCOMMANDTASK_H */
