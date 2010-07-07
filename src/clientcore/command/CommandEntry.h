#ifndef COMMANDENTRY_H
#define COMMANDENTRY_H

#include "CommandEntry.h"

#include <QString>

enum CommandEntryType {CMD_ONE_LINE, CMD_MULTI_LINE};

class CommandEntry {

    public:
        CommandEntry(const QString, const QString, CommandEntryType);
        CommandEntry();
        ~CommandEntry();

        // The short name of the plugin that owns this command
        const QString& pluginName() const;
        void pluginName(const QString);

	// The command to be registered
        const QString& command() const;
        void command(const QString);

	// The help information for this command
        const QString& help() const;
        void help(const QString);

	// The event data type to return to the plugin
        const QString& dataType() const;
        void dataType(const QString);

	// The type of command, one- or multi-lined
	CommandEntryType commandType() const;
	void commandType(CommandEntryType);

	// Is the command a locking semaphore?
	bool locking() const;
	void locking(bool);

    protected:
	bool _locking;
        QString _pluginName;
	QString _command;
	QString _help;
	QString _dataType;

	CommandEntryType _commandType;
};


#endif // COMMANDENTRY_H
