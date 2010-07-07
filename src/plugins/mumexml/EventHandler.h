#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MClientEventHandler.h"

class QEvent;
class QTimer;

class EventHandler : public MClientEventHandler {
    Q_OBJECT
    
    public:
         EventHandler(AbstractPluginSession *ps, MClientPlugin *mp);
        ~EventHandler();

        void customEvent(QEvent* e);

public slots:
        void postFragment();

 protected:
	static const QString emptyString;
	static const QChar escChar;
	/*
	static const QString nullString;
	static const QByteArray greaterThanChar;
	static const QByteArray lessThanChar;
	static const QByteArray greaterThanTemplate;
	static const QByteArray lessThanTemplate;
	*/

    private:
	void parse(const QByteArray&);
	bool element(const QByteArray&);
	bool characters(const QByteArray&);

	enum CharactersState {NORMAL, AMPERSAND, GREATER_THAN_1, LESS_THAN_1, GREATER_THAN_2, LESS_THAN_2};
	CharactersState _charState;

	enum XmlMode {XML_NONE, XML_PROMPT, XML_ROOM, XML_NAME, XML_DESCRIPTION, XML_TERRAIN, XML_MAGIC, XML_EXITS, XML_TELL, XML_SAY, XML_NARRATE, XML_SONG, XML_PRAY, XML_SHOUT, XML_YELL, XML_EMOTE, XML_HIT, XML_DAMAGE, XML_WEATHER, XML_VIEW, XML_VIEW_TITLE, XML_VIEW_BODY, XML_EDIT, XML_EDIT_TITLE, XML_EDIT_BODY, XML_HACK};
	XmlMode _xmlMode;

	QString _buffer;
	QTimer *_fragmentTimer;

	QByteArray _tempCharacters;
	QByteArray _tempTag;
	bool _readingTag;

	bool _removeXmlTags;

	void postBuffer(const QStringList &);
};


#endif /* EVENTHANDLER_H */
