#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientEventHandler.h"

class QEvent;

enum XmlMode {XML_NONE, XML_PROMPT, XML_ROOM, XML_NAME, XML_DESCRIPTION, XML_TERRAIN, XML_MAGIC, XML_EXITS, XML_TELL, XML_SAY, XML_NARRATE, XML_SONG, XML_PRAY, XML_SHOUT, XML_YELL, XML_EMOTE, XML_HIT, XML_DAMAGE, XML_WEATHER};

class EventHandler : public MClientEventHandler {
    Q_OBJECT
    
    public:
        EventHandler(QObject* parent=0);
        ~EventHandler();

        void customEvent(QEvent* e);

 protected:
	static const QString nullString;
	static const QString emptyString;
	static const QByteArray greaterThanChar;
	static const QByteArray lessThanChar;
	static const QByteArray greaterThanTemplate;
	static const QByteArray lessThanTemplate;
	static const QChar escChar;

    private:
	void parse(const QByteArray&);
	bool element(const QByteArray&);
	bool characters(QByteArray&);

	QString _buffer;

	QByteArray _tempCharacters;
	QByteArray _tempTag;
	bool _readingTag;
	XmlMode _xmlMode;

	bool _removeXmlTags;
};


#endif /* EVENTHANDLER_H */
