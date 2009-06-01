#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <QThread>
#include <QMap>
#include <QHash>
#include <QMultiHash>
#include <QString>
#include <QStringList>

typedef QString Source;
typedef QString DataType;
typedef QString Command;

class CommandManager : public QThread {
    Q_OBJECT
    
    public:
        static CommandManager* instance();
        void destroy();

        CommandManager(QObject* parent=0);
        ~CommandManager();

        void configure();
        const bool loadSettings();
        const bool saveSettings() const;

	void run();

	void parseInput(const QString&, const QString&);
	bool unregisterCommand(const QString &source);
	void registerCommand(const QStringList &sl);

    private:
        static CommandManager* _pinstance;

        QChar _symbol, _delim;
	QMap<Command, DataType> _mapping;
	QMultiHash<Source, Command> _registry;

        bool parseCommand(QString, const QString&, const QString&);
        void displayData(const QString&, const QString&);
        void postEvent(QVariant*, const QStringList&, const QString&);

 signals:
	void quit();
};


#endif /* COMMANDMANAGER_H */
