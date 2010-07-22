#include "EventHandler.h"

#include <QEvent>
#include <QVariant>
#include <QStringList>
#include <QTimer>

#include <QDebug>

#include "MClientEvent.h"
#include "AbstractPluginSession.h"

/*
const QByteArray EventHandler::greaterThanChar(">");
const QByteArray EventHandler::lessThanChar("<");
const QByteArray EventHandler::greaterThanTemplate("&gt;");
const QByteArray EventHandler::lessThanTemplate("&lt;");
const QString EventHandler::nullString;
*/
const QString EventHandler::emptyString("");
const QChar EventHandler::escChar('\x1B');

//const QByteArray EventHandler::emptyByteArray("");


EventHandler::EventHandler(AbstractPluginSession *ps, MClientPlugin *mp)
  : MClientEventHandler(ps, mp) {
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


EventHandler::~EventHandler() {
}


void EventHandler::customEvent(QEvent *e) {
  if (e->type() == 10001) {

    // Forward the event to the next in the chain
    forwardEvent(e);

    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    QStringList types = me->dataTypes();
    foreach(QString s, types) {
      if (s.startsWith("TelnetData")) {
	if (_xmlMode == XML_NONE)
	  _fragmentTimer->start(); // (re)start timer
	else
	  _fragmentTimer->stop(); // We don't need a fragment timer otherwise
	parse(me->payload()->toByteArray());

      }
      else if (s.startsWith("TelnetGA")) {
	_fragmentTimer->stop();
	qDebug() << "* Flushing buffer due to TelentGA:" << _buffer;
	postFragment();

      }
      else if (s.startsWith("SocketDisconnected")) {
	postFragment();
	_buffer.clear();

      }
      else if (s.startsWith("SocketConnected")) {
	if (_pluginSession->isMUME()) { 
	  qDebug() << "* sent mume MPI XML request";
	  QVariant* qv = new QVariant(QByteArray("~$#EX1\n1"));
	  QStringList sl("SendToSocketData");
	  postSession(qv, sl);

	}
      }
      
    }
  }
  else {
    qDebug() << "MumeXML somehow received the wrong kind of event...";
    
  }
}


void EventHandler::parse(const QByteArray& line) {
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
}


bool EventHandler::element(const QByteArray& line) {
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
      QStringList sl;
      sl << "XMLNone" << "XMLAll";
      postBuffer(sl);
      
    }
    // Match for the tag
    if (length > 0)
      switch (line.at(0)) {
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
	  QVariant* qv = new QVariant(key);
	  QStringList sl;
	  sl << "XMLEdit";
	  postSession(qv, sl);
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
	  QVariant* qv = new QVariant(move);
	  QStringList sl;
	  sl << "XMLMove";
	  postSession(qv, sl);
	  
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
	  QStringList sl;
	  sl << "XMLDynamicDescription" << "XMLAll";
	  postBuffer(sl);

	}
	break;
      case 't':
	if (line.startsWith("terrain")) _xmlMode = XML_TERRAIN;
      }
    break;
  case XML_NAME:
    if (line.startsWith("/name")) {
      _xmlMode = XML_ROOM;
      
      /** Mint's server doesn't place the newline after the name */
      if (_buffer.at(_buffer.size() - 1) != '\n')
	_buffer.append("\n");
      
      // Post Room Name Event
      QStringList sl;
      sl << "XMLName" << "XMLAll";
      postBuffer(sl);
      
    }
    break;
  case XML_DESCRIPTION:
    if (length > 0)
      switch (line.at(0)) {
      case '/':
	if (line.startsWith("/description")) {
	  _xmlMode = XML_ROOM;
	  
	  // Post Static Room Description Event
	  QStringList sl;
	  sl << "XMLDescription" << "XMLAll";
	  postBuffer(sl);

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
	  QStringList sl;
	  sl << "XMLExits" << "XMLAll";
	  postBuffer(sl);
	  
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
	    QStringList sl;
	    sl << "XMLPrompt" << "XMLAll";
	    postBuffer(sl);
	  
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
	  QStringList sl;
	  sl << "XMLTerrain" << "XMLAll";
	  postBuffer(sl);
	  
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
	  QStringList sl;
	  sl << "XMLMagic" << "XMLAll";
	  postBuffer(sl);
	  
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
	  QStringList sl;
	  sl << "XMLTell" << "XMLAll" << "XMLCommunication";
	  postBuffer(sl);
	  
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
	  QStringList sl;
	  sl << "XMLSay" << "XMLAll" << "XMLCommunication";
	  postBuffer(sl);
	  
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
	  QStringList sl;
	  sl << "XMLNarrate" << "XMLAll" << "XMLCommunication";
	  postBuffer(sl);
	  
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
	  QStringList sl;
	  sl << "XMLSong" << "XMLAll" << "XMLCommunication";
	  postBuffer(sl);
	  
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
	  QStringList sl;
	  sl << "XMLPray" << "XMLAll" << "XMLCommunication";
	  postBuffer(sl);
	  
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
	  QStringList sl;
	  sl << "XMLShout" << "XMLAll" << "XMLCommunication";
	  postBuffer(sl);
	  
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
	  QStringList sl;
	  sl << "XMLYell" << "XMLAll" << "XMLCommunication";
	  postBuffer(sl);
	  
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
	  QStringList sl;
	  sl << "XMLEmote" << "XMLAll" << "XMLCommunication";
	  postBuffer(sl);
	  
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
	  QStringList sl;
	  sl << "XMLDamage" << "XMLAll" << "XMLCombat";
	  postBuffer(sl);
	  
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
	  QStringList sl;
	  sl << "XMLHit" << "XMLAll" << "XMLCombat";
	  postBuffer(sl);
	  
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
	  QStringList sl;
	  sl << "XMLWeather" << "XMLAll";
	  postBuffer(sl);

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
	  QStringList sl;
	  sl << "XMLViewTitle";
	  postBuffer(sl);

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
	QByteArray fromBase64 = QByteArray::fromBase64(_buffer.toAscii());
	_buffer = fromBase64.data();

	// Post View/Body Event
	QStringList sl;
	sl << "XMLViewBody";
	postBuffer(sl);

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
	    QStringList sl;
	    sl << "XMLEditBody";
	    postBuffer(sl);
	    
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
	  QStringList sl;
	  sl << "XMLEditTitle";
	  postBuffer(sl);

	}
	break;
      }
    break;
  case XML_EDIT_BODY:
    switch (line.at(0)) {
    case '/':
      if (line.startsWith("/body")) {
	_xmlMode = XML_EDIT;
	QByteArray fromBase64 = QByteArray::fromBase64(_buffer.toAscii());
	_buffer = fromBase64.data();

	// Post Edit/Body Event
	QStringList sl;
	sl << "XMLEditBody";
	postBuffer(sl);

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
	QVariant *qv = new QVariant();
	QStringList sl;
	sl << "XMLExits" << "XMLAll";
	postSession(qv, sl);
		
      }
      break;
    default:
      _xmlMode = XML_NONE;
      qWarning() << "! XML_HACK saw an unknown tag:" << line;
    }
    // Post anything in the buffer
    if (!_buffer.isEmpty()) {
      QStringList sl;
      sl << "XMLNone" << "XMLAll";
      postBuffer(sl);
    }
    break;
  }

  return true;
}

bool EventHandler::characters(const QByteArray& data) {
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
	  QStringList sl;
	  sl << "XMLNone" << "XMLAll";
	  postBuffer(sl);
  
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


void EventHandler::postBuffer(const QStringList &tags) {
  QVariant *qv = new QVariant(_buffer);
  postSession(qv, tags);
  _buffer.clear();

}


void EventHandler::postFragment() {
  if (!_buffer.isEmpty()) {
    QStringList sl;
    sl << "XMLPrompt" << "XMLAll";
    postBuffer(sl); 
  }

}
