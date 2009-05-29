#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <QHash>
#include <QMultiHash>
#include <QString>
#include <QStringList>

typedef QString Source;
typedef QString DataType;
typedef QString Command;

class CommandManager : public QObject {
    Q_OBJECT
    
    public:
        static CommandManager* instance();
        void destroy();

        CommandManager(QObject* parent=0);
        ~CommandManager();

        void configure();
        const bool loadSettings();
        const bool saveSettings() const;

	bool parseInput(const QString&, const QString&);
	bool unregisterCommand(const QString& source);
	void registerCommand(const QStringList& sl);

    private:
        static CommandManager* _pinstance;

        QChar _symbol;
	QHash<Command, DataType> _mapping;
	QMultiHash<Source, Command> _registry;

        void postEvent(QVariant*, const QStringList&, const QString&);

};


#endif /* COMMANDMANAGER_H */
