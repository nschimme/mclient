#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <QObject>
#include <QMap>
#include <QMultiHash>
#include <QStringList>
#include <QEvent>

typedef QString Source;
typedef QString DataType;
typedef QString Command;

class CommandTask;
class PluginSession;

class CommandProcessor : public QObject {
    Q_OBJECT
    
    public:
        CommandProcessor(PluginSession*, QObject* parent=0);
        ~CommandProcessor();

	// For receiving events
	//void customEvent(QEvent* e);

        void configure();
        bool loadSettings();
        bool saveSettings() const;

	// For sending messages to
	QObject* getUserInput() const;
	QObject* getAction() const;

	//void parseInput(const QString&);
	bool unregisterCommand(const QString &source);
	void registerCommand(const QStringList &sl);

	QMap<Command, DataType> getCommandMapping() { return _mapping; };
	QChar getCommandSymbol() { return _symbol; };
	QChar getDelimSymbol() { return _delim; };
	PluginSession* getPluginSession() { return _pluginSession; };
	void emitQuit();

   private:
	/** Commands Section */
        QChar _symbol, _delim;
	QMap<Command, DataType> _mapping;
	QMultiHash<Source, Command> _registry;

	PluginSession *_pluginSession;
	CommandTask *_actionTask, *_userInputTask;

 signals:
	void quit();
};


#endif /* COMMANDPROCESSOR_H */
