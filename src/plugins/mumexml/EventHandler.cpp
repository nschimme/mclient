#include "EventHandler.h"

#include <QEvent>
#include <QVariant>
#include <QStringList>
#include <QDebug>

#include "MClientEvent.h"

const QByteArray EventHandler::greaterThanChar(">");
const QByteArray EventHandler::lessThanChar("<");
const QByteArray EventHandler::greaterThanTemplate("&gt;");
const QByteArray EventHandler::lessThanTemplate("&lt;");
const QString EventHandler::nullString;
const QString EventHandler::emptyString("");
const QChar EventHandler::escChar('\x1B');

//const QByteArray EventHandler::emptyByteArray("");


EventHandler::EventHandler(QObject* parent) : MClientEventHandler(parent) {
  _readingTag = false;
  _xmlMode = XML_NONE;
  _removeXmlTags = true;

  _buffer = emptyString;
}


EventHandler::~EventHandler() {
}


void EventHandler::customEvent(QEvent *e) {
  if (e->type() == 10000)
    engineEvent(e);

  else if (e->type() == 10001) {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    QStringList types = me->dataTypes();
    foreach(QString s, types) {
      if (s.startsWith("TelnetData")) {
	parse(me->payload()->toByteArray());

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
        if (!_tempTag.isEmpty())
          element( _tempTag );

        _tempTag.clear();

        _readingTag = false;
        continue;
      }
      _tempTag.append(line.at(index));
    }
    else
    {
      if (line.at(index) == '<') {
	/*
        //send characters (ignore whitespace as this can be pretty print)
	QRegExp rx("^\\s+?(\n.+)");
	rx.setMinimal(true);
	QString temp(_tempCharacters);
	temp.replace(rx, "\\1");
	_tempCharacters = temp.toLatin1();
	*/
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
    qDebug() << "##line" << line << ansi << _buffer.length();
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
    qDebug() << "##ansi2" << ansi;
    qDebug() << "##buffer" << _buffer << _buffer.length();
    return true;
  }
  
  switch (_xmlMode) {
  case XML_NONE:
    if (length > 0)
      switch (line.at(0)) {
      case 'p':
	if (line.startsWith("prompt")) _xmlMode = XML_PROMPT;
	else
          if (line.startsWith("pray")) _xmlMode = XML_PRAY;
	break;
      case 'e':
	if (line.startsWith("exits")) _xmlMode = XML_EXITS;
	else
          if (line.startsWith("emote")) _xmlMode = XML_EMOTE;
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
	else
	  if (line.startsWith("song")) _xmlMode = XML_SONG;
	  else
	    if (line.startsWith("shout")) _xmlMode = XML_SHOUT;
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
      case '/':
	if (line.startsWith("/xml")) {
	  qDebug() << "! XML mode disabled on MUME";
	}
	break;
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
	  _xmlMode = XML_NONE;
	  
	  // Post Dynamic Description Event
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLDynamicDescription" << "XMLAll";
	  postSession(qv, sl);
	  
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
      
      /** Mint's server doesn't place the newbline after the name */
      if (_buffer.at(_buffer.size() - 1) != '\n')
	_buffer.append("\n");
      
      // Post Room Name Event
      QVariant* qv = new QVariant(_buffer);
      QStringList sl;
      sl << "XMLName" << "XMLAll";
      postSession(qv, sl);
      
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
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLDescription" << "XMLAll";
	  postSession(qv, sl);
	  
	  _buffer.clear(); // reset buffer for dynamic desc
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
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLExits" << "XMLAll";
	  postSession(qv, sl);
	  
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
	    QVariant* qv = new QVariant(_buffer);
	    QStringList sl;
	    sl << "XMLPrompt" << "XMLAll";
	    postSession(qv, sl);
	    
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
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLTerrain" << "XMLAll";
	  postSession(qv, sl);
	  
	  _buffer.clear();
	  
	}
        break;
      }
  case XML_MAGIC:
    if (length > 0)
      switch (line.at(0)) {
      case '/':
	if (line.startsWith("/magic")) {
	  _xmlMode = XML_NONE;
	  
	  // Post Magic Event
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLMagic" << "XMLAll";
	  postSession(qv, sl);
	  
	  _buffer.clear();
	  
	}
        break;
      }
  case XML_TELL:
    if (length > 0)
      switch (line.at(0)) {
      case '/':
	if (line.startsWith("/tell")) {
	  _xmlMode = XML_NONE;
	  
	  // Post Tell Event
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLTell" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);
	  
	  _buffer.clear();
	  
	}
        break;
      }
  case XML_SAY:
    if (length > 0)
      switch (line.at(0)) {
      case '/':
	if (line.startsWith("/say")) {
	  _xmlMode = XML_NONE;
	  
	  // Post Say Event
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLSay" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);
	  
	  _buffer.clear();
	  
	}
        break;
      }
  case XML_NARRATE:
    if (length > 0)
      switch (line.at(0)) {
      case '/':
	if (line.startsWith("/narrate")) {
	  _xmlMode = XML_NONE;
	  
	  // Post Narrate Event
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLNarrate" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);

	  _buffer.clear();
	  
	}
        break;
      }
  case XML_SONG:
    if (length > 0)
      switch (line.at(0)) {
      case '/':
	if (line.startsWith("/song")) {
	  _xmlMode = XML_NONE;
	  
	  // Post Song Event
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLSong" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);
	  
	  _buffer.clear();
	  
	}
        break;
      }
  case XML_PRAY:
    if (length > 0)
      switch (line.at(0)) {
      case '/':
	if (line.startsWith("/pray")) {
	  _xmlMode = XML_NONE;
	  
	  // Post Pray Event
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLPray" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);
	  
	  _buffer.clear();
	  
	}
        break;
      }
  case XML_SHOUT:
    if (length > 0)
      switch (line.at(0)) {
      case '/':
	if (line.startsWith("/shout")) {
	  _xmlMode = XML_NONE;
	  
	  // Post Shout Event
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLShout" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);
	  
	  _buffer.clear();
	  
	}
        break;
      }
  case XML_YELL:
    if (length > 0)
      switch (line.at(0)) {
      case '/':
	if (line.startsWith("/yell")) {
	  _xmlMode = XML_NONE;
	  
	  // Post Yell Event
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLYell" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);
	  
	  _buffer.clear();
	  
	}
        break;
      }
  case XML_EMOTE:
    if (length > 0)
      switch (line.at(0)) {
      case '/':
	if (line.startsWith("/emote")) {
	  _xmlMode = XML_NONE;
	  
	  // Post Emote Event
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLEmote" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);
	  
	  _buffer.clear();
	  
	}
        break;
      }
  case XML_DAMAGE:
    if (length > 0)
      switch (line.at(0)) {
      case '/':
	if (line.startsWith("/damage")) {
	  _xmlMode = XML_NONE;
	  
	  // Post Damage Event
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLDamage" << "XMLAll" << "XMLCombat";
	  postSession(qv, sl);
	  
	  _buffer.clear();
	  
	}
        break;
      }
  case XML_HIT:
    if (length > 0)
      switch (line.at(0)) {
      case '/':
	if (line.startsWith("/hit")) {
	  _xmlMode = XML_NONE;
	  
	  // Post Hit Event
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLHit" << "XMLAll" << "XMLCombat";
	  postSession(qv, sl);
	  
	  _buffer.clear();
	  
	}
        break;
      }
  case XML_WEATHER:
    if (length > 0)
      switch (line.at(0)) {
      case '/':
	if (line.startsWith("/weather")) {
	  _xmlMode = XML_NONE;
	  
	  // Post Weather Event
	  QVariant* qv = new QVariant(_buffer);
	  QStringList sl;
	  sl << "XMLWeather" << "XMLAll";
	  postSession(qv, sl);

	  _buffer.clear();

	}
        break;
      }
    break;
  }
  
  if (!_removeXmlTags) {
    QString output = "<"+line+">";
    QVariant* qv = new QVariant(output);
    QStringList sl("XMLTag");
    postSession(qv, sl);
  }
  return true;
}

bool EventHandler::characters(QByteArray& ch) {
  // replace > and < chars
  ch.replace(greaterThanTemplate, greaterThanChar);
  ch.replace(lessThanTemplate, lessThanChar);

  QVariant *qv;
  QStringList sl;

  switch (_xmlMode) {
  case XML_NONE:
    qv = new QVariant(ch);
    sl << "XMLNone" << "XMLAll";
    postSession(qv, sl);
    break;
    
  case XML_ROOM: // dynamic line
  case XML_NAME:
  case XML_DESCRIPTION: // static line
  case XML_EXITS:
  case XML_PROMPT:
  default:
    // This is for single-lined XML tags.
    if (ch.length() > 0)
      _buffer.append(ch);
    break;
  }
  
  return true;
}
