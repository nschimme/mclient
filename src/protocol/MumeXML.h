#ifndef MUMEXML_H
#define MUMEXML_H

#include <QObject>

class QTimer;
class Session;

class MumeXML : public QObject {
    Q_OBJECT
    
public:
    MumeXML(Session *session);
    ~MumeXML();

signals:
    void sendMpiXmlRequest(const QByteArray&);
    void xmlNone(const QString&);
    void xmlDynamicDescription(const QString&);
    void xmlRoomName(const QString&);
    void xmlRoomDescription(const QString&);
    void xmlRoomExits(const QString&);
    void xmlPrompt(const QString&);
    void xmlTerrain(const QString&);
    void xmlMagic(const QString&);
    void xmlTell(const QString&);
    void xmlNarrate(const QString&);
    void xmlSong(const QString&);
    void xmlPray(const QString&);
    void xmlShout(const QString&);
    void xmlYell(const QString&);
    void xmlEmote(const QString&);
    void xmlSay(const QString&);
    void xmlDamage(const QString&);
    void xmlAvoidDamage(const QString&);
    void xmlHit(const QString&);
    void xmlWeather(const QString&);
    void xmlMovement(const QString&);
    void xmlViewTitle(const QString&);
    void xmlViewBody(const QString&);
    void xmlEditBody(const QString&);
    void xmlEditTitle(const QString&);
    void xmlEditKey(const int);

public slots:
    void parse(const QString& data, bool goAhead);
    void socketConnected();

protected slots:
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
    bool element(const QByteArray&);
    bool characters(const QByteArray&);

    enum CharactersState {NORMAL, AMPERSAND, GREATER_THAN_1, LESS_THAN_1, GREATER_THAN_2, LESS_THAN_2};
    CharactersState _charState;

    enum XmlMode {XML_NONE, XML_PROMPT, XML_ROOM, XML_NAME, XML_DESCRIPTION, XML_TERRAIN, XML_MAGIC, XML_EXITS, XML_TELL, XML_SAY, XML_NARRATE, XML_SONG, XML_PRAY, XML_SHOUT, XML_YELL, XML_EMOTE, XML_HIT, XML_DAMAGE, XML_AVOID_DAMAGE, XML_WEATHER, XML_VIEW, XML_VIEW_TITLE, XML_VIEW_BODY, XML_EDIT, XML_EDIT_TITLE, XML_EDIT_BODY, XML_HACK};
    XmlMode _xmlMode;

    QString _buffer;
    QTimer *_fragmentTimer;

    QByteArray _tempCharacters;
    QByteArray _tempTag;
    bool _readingTag;
    bool _removeXmlTags;
};


#endif /* MUMEXML_H */
