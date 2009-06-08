#ifndef MMAPPERPLUGIN_H
#define MMAPPERPLUGIN_H

#include "MClientDisplayPlugin.h"

#include <QHash>

class MMapperPluginParser;
class MapperManager;
class QEvent;

class MMapperPlugin : public MClientDisplayPlugin {
    Q_OBJECT

    public:
        MMapperPlugin(QWidget* parent=0);
        ~MMapperPlugin();

        // Plugin members
        void customEvent(QEvent* e);
        void configure();
        const bool loadSettings();
        const bool saveSettings() const;
        const bool startSession(QString s);
        const bool stopSession(QString s);

        // Display members
        const bool initDisplay(QString s);
        QWidget* getWidget(QString s);

public slots:
	void displayMessage(const QString&, const QString&);

    private:
	QHash<QString, QString> *_settings;
        QHash<QString, MapperManager*> _mappers;
	QHash<QString, MMapperPluginParser*> _parsers;

 signals:
	void name(QString, const QString &);
	void description(const QString &, const QString &);
	void dynamicDescription(const QString &, const QString &);
	void prompt(QString, const QString &);
	void exits(QString, const QString &);
	void move(QString, const QString &);

	void userInput(const QString&, const QString&);
	void mudOutput(const QString&, const QString&);

	// TODO: add session support
	void loadFile(const QString&);
};


#endif /* MMAPPERPLUGIN_H */
