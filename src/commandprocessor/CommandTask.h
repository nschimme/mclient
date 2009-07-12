#ifndef COMMANDTASK_H
#define COMMANDTASK_H

#include <QRunnable>
#include <QStringList>

enum CommandProcessorType { COMMAND_ALIAS = 0, COMMAND_ACTION };

class CommandProcessor;

class CommandTask : public QRunnable {
    
    public:
        CommandTask(const QString&, CommandProcessor*);
        CommandTask(const QString&, const QStringList&, CommandProcessor*);
        ~CommandTask();

	void run();

   private:
	CommandProcessorType _type;
	QString _input;
	QStringList _queue, _tags;
	uint _stack;
	CommandProcessor *_commandProcessor;

        void parseInput(const QString&, const QChar& =QChar('\n'));
	void processStack();
	
        bool parseCommand(QString, const QString&);

	bool handleAliasCommand(const QString&);
	QString findAlias(const QString&, const QString&);

	bool handleActionCommand(const QString&);
	bool findAction(const QString&, QStringList);

	void displayData(const QString&);
	void postSession(QVariant*, const QStringList&);
};


#endif /* COMMANDTASK_H */
