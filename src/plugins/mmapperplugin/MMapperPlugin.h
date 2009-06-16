#ifndef MMAPPERPLUGIN_H
#define MMAPPERPLUGIN_H

#include "MClientDisplayPlugin.h"

#include <QQueue>
#include <QHash>

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
        bool loadSettings();
        bool saveSettings() const;
        bool startSession(QString s);
        bool stopSession(QString s);

        // Display members
        bool initDisplay(QString s);
        QWidget* getWidget(QString s);

public slots:
	void displayMessage(const QString&);
        void log(const QString&, const QString&);

    private:
	QHash<QString, QString> *_settings;
        MapperManager* _mapper;

 signals:
	void name(QString);
	void description(const QString &);
	void dynamicDescription(const QString &);
	void prompt(QString);
	void exits(QString);
	void move(QString);

	void userInput(QString);
	void mudOutput(const QString&);

	void loadFile(const QString&);
};


#endif /* MMAPPERPLUGIN_H */
