#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <QThread>
#include <QMap>
#include <QMultiHash>
#include <QStringList>
#include <QEvent>

typedef QString Source;
typedef QString DataType;
typedef QString Command;

class PluginSession;

class CommandProcessor : public QThread {
    Q_OBJECT
    
    public:
        CommandProcessor(PluginSession*, QObject* parent=0);
        ~CommandProcessor();

	// For receiving events
	void customEvent(QEvent* e);

        void configure();
        bool loadSettings();
        bool saveSettings() const;

	void parseInput(const QString&);
	bool unregisterCommand(const QString &source);
	void registerCommand(const QStringList &sl);

	QMap<Command, DataType> getCommandMapping() { return _mapping; };
	QChar getCommandSymbol() { return _symbol; };
	QChar getDelimSymbol() { return _delim; };
	PluginSession* getPluginSession() { return _pluginSession; };
	void emitQuit();

 protected:
	void run();

   private:
	/** Commands Section */
        QChar _symbol, _delim;
	QMap<Command, DataType> _mapping;
	QMultiHash<Source, Command> _registry;

	PluginSession *_pluginSession;

 signals:
	void quit();
};


#endif /* COMMANDPROCESSOR_H */
