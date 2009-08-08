#ifndef COMMANDTASK_H
#define COMMANDTASK_H

#include <QThread>
#include <QStringList>

enum CommandProcessorType { COMMAND_ALIAS = 0, COMMAND_ACTION };

class CommandProcessor;

class CommandTask : public QThread {
    
    public:
        CommandTask(const CommandProcessorType&, CommandProcessor*,
		    QObject *parent=0);
        ~CommandTask();

	void customEvent(QEvent *e);

 protected:
	void run();

   private:
	CommandProcessorType _type;
	QStringList _queue;
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
