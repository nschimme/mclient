#ifndef COMMANDENTRY_H
#define COMMANDENTRY_H

#include "CommandEntry.h"

#include <QString>

class CommandEntry {

    public:
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

    protected:
        QString _pluginName;
	QString _command;
	QString _help;
	QString _dataType;
};


#endif // COMMANDENTRY_H
