#ifndef MUMEXML_H
#define MUMEXML_H

#include "MClientFilterPlugin.h"

#include <QQueue>
#include <QMutex>
#include <QWaitCondition>

class QByteArray;
class MClientEvent;
class QEvent;

enum XmlMode {XML_NONE, XML_PROMPT, XML_ROOM, XML_NAME, XML_DESCRIPTION, XML_TERRAIN, XML_MAGIC, XML_EXITS, XML_TELL, XML_SAY, XML_NARRATE, XML_SONG, XML_PRAY, XML_SHOUT, XML_YELL, XML_EMOTE, XML_HIT, XML_DAMAGE, XML_WEATHER};

class MumeXML : public MClientFilterPlugin {
    Q_OBJECT
    
    public:
        MumeXML(QObject* parent=0);
        ~MumeXML();

        // From MClientFilterPlugin
        void customEvent(QEvent* e);
        void configure();
        bool loadSettings();
        bool saveSettings() const;
        bool startSession(QString s);
        bool stopSession(QString s);

 protected:
        void run();

	static const QString nullString;
	static const QString emptyString;
	static const QByteArray greaterThanChar;
	static const QByteArray lessThanChar;
	static const QByteArray greaterThanTemplate;
	static const QByteArray lessThanTemplate;

    private:
	void parse(const QByteArray&);
	bool element(const QByteArray&);
	bool characters(QByteArray&);

	QString _singleBuffer;
	QString _multiBuffer;

	QByteArray _tempCharacters;
	QByteArray _tempTag;
	bool _readingTag;
	XmlMode _xmlMode;

	bool _removeXmlTags;

	bool _quit;
	QMutex _mutex;
	QWaitCondition _cond;
	QQueue< QByteArray > _eventQueue;
};


#endif /* MUMEXML_H */
