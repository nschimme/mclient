#ifndef MUMEXML_H
#define MUMEXML_H

#include "MClientFilterPlugin.h"

#include <QQueue>

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
        const bool loadSettings();
        const bool saveSettings() const;
        const bool startSession(QString s);
        const bool stopSession(QString s);
        void run();

 protected:
	static const QString nullString;
	static const QString emptyString;
	static const QByteArray greatherThanChar;
	static const QByteArray lessThanChar;
	static const QByteArray greatherThanTemplate;
	static const QByteArray lessThanTemplate;

    private:
	void parse(const QByteArray&, const QString&);
	bool element(const QByteArray&, const QString&);
	bool characters(QByteArray&, const QString&);

	QString _singleBuffer;
	QString _multiBuffer;
	bool _readingRoomDesc;
	bool _descriptionReady;

	QByteArray _tempCharacters;
	QByteArray _tempTag;
	bool _readingTag;
	XmlMode _xmlMode;

	bool _removeXmlTags;

	QQueue< QPair<QByteArray, QString> > _eventQueue;
};


#endif /* MUMEXML_H */
