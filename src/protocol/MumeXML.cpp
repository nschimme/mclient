#include "MumeXML.h"
#include "Session.h"

#include <QEvent>
#include <QVariant>
#include <QStringList>
#include <QTimer>

#include <QDebug>

/*
const QByteArray MumeXML::greaterThanChar(">");
const QByteArray MumeXML::lessThanChar("<");
const QByteArray MumeXML::greaterThanTemplate("&gt;");
const QByteArray MumeXML::lessThanTemplate("&lt;");
const QString MumeXML::nullString;
*/
const QString MumeXML::emptyString("");
const QChar MumeXML::escChar('\x1B');

//const QByteArray MumeXML::emptyByteArray("");


MumeXML::MumeXML(Session *session)
    : QObject(session) {
    _readingTag = false;
    _xmlMode = XML_NONE;
    _removeXmlTags = true;

    _charState = NORMAL;

    _buffer = emptyString;

    // Timer
    _fragmentTimer = new QTimer(this);
    _fragmentTimer->setInterval(500); // 500 ms
    _fragmentTimer->setSingleShot(true);
    connect(_fragmentTimer, SIGNAL(timeout()), this, SLOT(postFragment()));

}


MumeXML::~MumeXML() {
}

void MumeXML::socketConnected() {
    qDebug() << "* sent mume MPI XML request";
    emit sendMpiXmlRequest(QByteArray("~$#EX1\n1\n~$#EP2\nG\n"));
}

void MumeXML::parse(const QString& line, bool goAhead) {
    if (_xmlMode != XML_NONE) {
        _fragmentTimer->stop();
    } else {
        _fragmentTimer->start(); // (re)start timer
    }
    //qDebug() << "* parsing xml from '" << line.toLatin1() << "' with goahead=" << goAhead << " and state=" << _xmlMode;

    int index;
    for (index = 0; index < line.size(); index++) {
        if (_readingTag) {
            if (line.at(index) == '>') {
                // Parse line according to the element's tag
                if (!_tempTag.isEmpty()) {
                    // Display tags?
                    if (!_removeXmlTags) _buffer.append("<" + _tempTag + ">");

                    element( _tempTag );
                }

                _tempTag.clear();

                _readingTag = false;
                continue;
            }
            _tempTag.append(line.at(index));
        }
        else
        {
            if (line.at(index) == '<') {
                //*
                //send characters (ignore whitespace as this can be pretty print)
                QRegExp rx("^\\s+?(\n.+)");
                rx.setMinimal(true);
                QString temp(_tempCharacters);
                temp.replace(rx, "\\1");
                _tempCharacters = temp.toLatin1();
                // */
                if (!_tempCharacters.isEmpty())
                    characters( _tempCharacters );
                _tempCharacters.clear();

                _readingTag = true;
                continue;
            }
            _tempCharacters.append(line.at(index));
        }
    }

    if (!_readingTag) {
        //send characters
        if (!_tempCharacters.isEmpty())
            characters( _tempCharacters );
        _tempCharacters.clear();
    }

    if (goAhead) {
        qDebug() << "* Flushing buffer due to TelentGA:" << _buffer;
        _fragmentTimer->stop();
        if (!_buffer.isEmpty()) {
            emit xmlNone(_buffer);
            _buffer.clear();

        }
    }
}


bool MumeXML::element(const QByteArray& line) {
    int length = line.length();

    // Mint's Server ANSI tag --> ANSI Codes
    if (line.startsWith("ansi")) {
        QByteArray ansi;
        ansi.append(escChar);
        ansi.append('[');
        switch (length) {
        case 13:
            ansi.append(line[9]);
            ansi.append(line[10]);
            ansi.append('m');
            break;
        case 12:
            ansi.append(line[9]);
            ansi.append('m');
            break;
        };
        _buffer.append(ansi);
        return true;
    }

    switch (_xmlMode) {
    case XML_NONE:
        // We found the beginning of a tag, post the XML_NONE buffer since
        // it is not a fragment
        if (!_buffer.isEmpty()) {
            emit xmlNone(_buffer);
            _buffer.clear();

        }
        // Match for the tag
        if (length > 0)
            switch (line.at(0)) {
            case 'a':
                if (line.startsWith("avoid_damage")) _xmlMode = XML_AVOID_DAMAGE;
                break;
            case 'p':
                if (line.startsWith("prompt")) _xmlMode = XML_PROMPT;
                else if (line.startsWith("pray")) _xmlMode = XML_PRAY;
                break;
            case 'e':
                if (line.startsWith("exits")) _xmlMode = XML_EXITS;
                else if (line.startsWith("emote")) _xmlMode = XML_EMOTE;
                else if (line.startsWith("edit")) {
                    _xmlMode = XML_EDIT;

                    // Post the XML Edit Key
                    int key = -1;
                    QRegExp rx("key=(\\d+)");
                    if (rx.indexIn(line) != -1) key = rx.capturedTexts().at(1).toInt();
                    emit xmlEditKey(key);

                }
                break;
            case 'r':
                if (line.startsWith("room")) _xmlMode = XML_ROOM;
                break;
            case 'm':
                if (line.startsWith("movement")) {
                    QString move;
                    if (length > 8)
                        switch (line.at(8)) {
                        case ' ':
                            if (length > 13)
                                switch (line.at(13)) {
                                case 'n':
                                    move = "north";
                                    break;
                                case 's':
                                    move = "south";
                                    break;
                                case 'e':
                                    move = "east";
                                    break;
                                case 'w':
                                    move = "west";
                                    break;
                                case 'u':
                                    move = "up";
                                    break;
                                case 'd':
                                    move = "down";
                                    break;
                                };
                            break;
                        case '/':
                            move = "unknown";
                            break;
                        };
                    // Post the XML Movement
                    emit xmlMovement(_buffer);
                    _buffer.clear();

                }
                else
                    if (line.startsWith("magic")) _xmlMode = XML_MAGIC;
                break;
            case 'd':
                if (line.startsWith("damage")) _xmlMode = XML_DAMAGE;
                break;
            case 'h':
                if (line.startsWith("hit")) _xmlMode = XML_HIT;
                break;
            case 's':
                if (line.startsWith("say")) _xmlMode = XML_SAY;
                else if (line.startsWith("song")) _xmlMode = XML_SONG;
                else if (line.startsWith("shout")) _xmlMode = XML_SHOUT;
                break;
            case 'n':
                if (line.startsWith("narrate")) _xmlMode = XML_NARRATE;
                break;
            case 'y':
                if (line.startsWith("yell")) _xmlMode = XML_YELL;
                break;
            case 't':
                if (line.startsWith("tell")) _xmlMode = XML_TELL;
                break;
            case 'w':
                if (line.startsWith("weather")) _xmlMode = XML_WEATHER;
                break;
            case 'v':
                if (line.startsWith("view")) _xmlMode = XML_VIEW;
                break;
            case 'x':
                if (line.startsWith("xml")) {
                    qDebug() << "! XML mode enabled on MUME";
                    _xmlMode = XML_NONE;
                    _buffer.clear();
                    _fragmentTimer->stop();
                }
                break;
            case '/':
                if (line.startsWith("/xml")) {
                    qDebug() << "! XML mode disabled on MUME";
                }
                break;
            default:
                qDebug() << "! Unknown XML tag:" << line;
            };
        break;
    case XML_ROOM:
        if (length > 0)
            switch (line.at(0)) {
            case 'n':
                if (line.startsWith("name")) _xmlMode = XML_NAME;
                break;
            case 'd':
                if (line.startsWith("description")) _xmlMode = XML_DESCRIPTION;
                break;
            case '/':
                if (line.startsWith("/room")) {
                    // Technically it should always move to XML_NONE, but this
                    // ensures that we submit an empty XML_EXITS since exits
                    // usually follow a room
                    _xmlMode = XML_HACK;

                    // Post Dynamic Description Event
                    emit xmlDynamicDescription(_buffer);
                    _buffer.clear();

                }
                break;
            case 't':
                if (line.startsWith("terrain")) _xmlMode = XML_TERRAIN;
            }
        break;
    case XML_NAME:
        if (line.startsWith("/name")) {
            _xmlMode = XML_ROOM;

            // Post Room Name Event
            emit xmlRoomName(_buffer);
            _buffer.clear();

        }
        break;
    case XML_DESCRIPTION:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/description")) {
                    _xmlMode = XML_ROOM;

                    // Post Static Room Description Event
                    emit xmlRoomDescription(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_EXITS:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/exits")) {
                    _xmlMode = XML_NONE;

                    // Post Room Exits Event
                    emit xmlRoomExits(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_PROMPT:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/prompt")) {
                    _xmlMode = XML_NONE;

                    // Post Prompt Event
                    emit xmlPrompt(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_TERRAIN:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/terrain")) {
                    _xmlMode = XML_ROOM;

                    // Post Terrain Event
                    emit xmlTerrain(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_MAGIC:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/magic")) {
                    _xmlMode = XML_NONE;

                    // Post Magic Event
                    emit xmlMagic(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_TELL:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/tell")) {
                    _xmlMode = XML_NONE;

                    // Post Tell Event
                    emit xmlTell(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_SAY:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/say")) {
                    _xmlMode = XML_NONE;

                    // Post Say Event
                    emit xmlSay(_buffer);
                    _buffer.clear();


                }
                break;
            }
        break;
    case XML_NARRATE:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/narrate")) {
                    _xmlMode = XML_NONE;

                    // Post Narrate Event
                    emit xmlNarrate(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_SONG:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/song")) {
                    _xmlMode = XML_NONE;

                    // Post Song Event
                    emit xmlSong(_buffer);
                    _buffer.clear();


                }
                break;
            }
        break;
    case XML_PRAY:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/pray")) {
                    _xmlMode = XML_NONE;

                    // Post Pray Event
                    emit xmlPray(_buffer);
                    _buffer.clear();


                }
                break;
            }
        break;
    case XML_SHOUT:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/shout")) {
                    _xmlMode = XML_NONE;

                    // Post Shout Event
                    emit xmlShout(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_YELL:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/yell")) {
                    _xmlMode = XML_NONE;

                    // Post Yell Event
                    emit xmlYell(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_EMOTE:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/emote")) {
                    _xmlMode = XML_NONE;

                    // Post Emote Event
                    emit xmlEmote(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_DAMAGE:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/damage")) {
                    _xmlMode = XML_NONE;

                    // Post Damage Event
                    emit xmlDamage(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_AVOID_DAMAGE:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/avoid_damage")) {
                    _xmlMode = XML_NONE;

                    // Post Avoid Damage Event
                    emit xmlAvoidDamage(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_HIT:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/hit")) {
                    _xmlMode = XML_NONE;

                    // Post Hit Event
                    emit xmlHit(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_WEATHER:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/weather")) {
                    _xmlMode = XML_NONE;

                    // Post Weather Event
                    emit xmlWeather(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_VIEW:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/view")) _xmlMode = XML_NONE;
                break;
            case 't':
                if (line.startsWith("title")) {
                    _xmlMode = XML_VIEW_TITLE;
                    // TODO md5 checksum storage
                }
                break;
            case 'b':
                if (line.startsWith("body")) {
                    _xmlMode = XML_VIEW_BODY;
                    // TODO md5 checksum storage
                }
                break;
            }
        break;
    case XML_VIEW_TITLE:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/title")) {
                    _xmlMode = XML_VIEW;
                    // TODO md5 checksum (requires attributes)

                    // Post View/Title Event
                    emit xmlViewTitle(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_VIEW_BODY:
        switch (line.at(0)) {
        case '/':
            if (line.startsWith("/body")) {
                _xmlMode = XML_VIEW;
                // TODO md5 checksum (requires attributes)
                QByteArray fromBase64 = QByteArray::fromBase64(_buffer.toLatin1());
                _buffer = fromBase64.data();

                // Post View/Body Event
                emit xmlViewBody(_buffer);
                _buffer.clear();

            }
            break;
        }
        break;
    case XML_EDIT:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/edit")) _xmlMode = XML_NONE;
                break;
            case 't':
                if (line.startsWith("title")) _xmlMode = XML_EDIT_TITLE;
                break;
            case 'b':
                if (line.startsWith("body")) {
                    // Check whether this is a single tag	(i.e. <body ... />)
                    if (line.at(line.size()-1) == '/') {
                        // Post Edit/Body Event
                        emit xmlEditBody(_buffer);
                        _buffer.clear();

                    }
                    else _xmlMode = XML_EDIT_BODY;
                }
                break;
            }
        break;
    case XML_EDIT_TITLE:
        if (length > 0)
            switch (line.at(0)) {
            case '/':
                if (line.startsWith("/title")) {
                    _xmlMode = XML_EDIT;

                    // Post Edit/Title Event
                    emit xmlEditTitle(_buffer);
                    _buffer.clear();

                }
                break;
            }
        break;
    case XML_EDIT_BODY:
        switch (line.at(0)) {
        case '/':
            if (line.startsWith("/body")) {
                _xmlMode = XML_EDIT;
                QByteArray fromBase64 = QByteArray::fromBase64(_buffer.toLatin1());
                _buffer = fromBase64.data();

                // Post Edit/Body Event
                emit xmlEditBody(_buffer);
                _buffer.clear();

            }
            break;
        }
        break;
    case XML_HACK:
        switch (line.at(0)) {
        case 'e':
            if (line.startsWith("exits")) _xmlMode = XML_EXITS;
            break;
        case 'p':
            if (line.startsWith("prompt")) {
                _xmlMode = XML_PROMPT;

                // Post an Empty Exits Event (this tag doesn't really exist)
                emit xmlRoomExits(_buffer);
                _buffer.clear();

            }
            break;
        default:
            if (!line.startsWith("xml")) {
                _xmlMode = XML_NONE;
                qWarning() << "! XML_HACK saw an unknown tag:" << line;
            }
        }
        // Post anything in the buffer
        if (!_buffer.isEmpty()) {
            emit xmlNone(_buffer);
            _buffer.clear();
        }
        break;
    }

    return true;
}

bool MumeXML::characters(const QByteArray& data) {
    // replace > (&gt;) and < (&lt;) chars and handle fragments
    for (unsigned int i = 0; i < (unsigned int) data.length(); i++) {
        switch (_charState) {
        case NORMAL:
            switch (data.at(i)) {
            case '&':
                // This might be a > or < character
                _charState = AMPERSAND;
                break;
            case '\n':
                // All other unknown tags get their contents printed still so
                // we add the current character to the buffer
                _buffer.append(data.at(i));

                // Post the buffer immediately if this is XML_NONE since it is
                // clearly not a fragment
                if (_xmlMode == XML_NONE) {
                    emit xmlNone(_buffer);
                    _buffer.clear();

                }
                break;
            default:
                // Add the current character to the buffer
                _buffer.append(data.at(i));
            };
            break;

        case AMPERSAND:
            switch (data.at(i)) {
            case 'l':
                _charState = LESS_THAN_1;
                break;
            case 'g':
                _charState = GREATER_THAN_1;
                break;
            default:
                _charState = NORMAL;
                _buffer.append(data.mid(i - 1, 2));
            };
            break;

        case GREATER_THAN_1:
            switch (data.at(i)) {
            case 't':
                _charState = GREATER_THAN_2;
                break;
            default:
                _charState = NORMAL;
                _buffer.append(data.mid(i - 2, 3));
            };
            break;

        case LESS_THAN_1:
            switch (data.at(i)) {
            case 't':
                _charState = LESS_THAN_2;
                break;
            default:
                _charState = NORMAL;
                _buffer.append(data.mid(i - 2, 3));
            };
            break;

        case GREATER_THAN_2:
            _charState = NORMAL;
            switch (data.at(i)) {
            case ';':
                _buffer.append('>');
                break;
            default:
                _buffer.append(data.mid(i - 3, 4));
            };
            break;

        case LESS_THAN_2:
            _charState = NORMAL;
            switch (data.at(i)) {
            case ';':
                _buffer.append('<');
                break;
            default:
                _buffer.append(data.mid(i - 3, 4));
            };
            break;
        };
    }

    return true;
}

void MumeXML::postFragment() {
    if (!_buffer.isEmpty()) {
        qDebug() << "* xml timer ran out, posting fragment: " << _buffer;
        emit xmlNone(_buffer);
        _buffer.clear();
    }
}
