#include "MumeXML.h"

#include "MClientEvent.h"
#include "MClientEngineEvent.h"
#include "PluginManager.h"

#include <QApplication>
#include <QDebug>
#include <QStringList>
#include <QVariant>

Q_EXPORT_PLUGIN2(mumexml, MumeXML)

const QByteArray MumeXML::greaterThanChar(">");
const QByteArray MumeXML::lessThanChar("<");
const QByteArray MumeXML::greaterThanTemplate("&gt;");
const QByteArray MumeXML::lessThanTemplate("&lt;");
const QString MumeXML::nullString;
const QString MumeXML::emptyString("");
//const QChar MumeXML::escChar('\x1B');
//const QByteArray MumeXML::emptyByteArray("");


MumeXML::MumeXML(QObject* parent) 
        : MClientFilterPlugin(parent) {
    _shortName = "mumexmlfilter";
    _longName = "Mume XML Filter";
    _description = "Filters the XML tags.";
    _dependencies.insert("telnet", 1);
    _implemented.insert("mumexml",1);
    _receivesDataTypes << "TelnetData";
    _deliversDataTypes << "XMLNone" << "XMLAll" << "XMLTag"

		       << "XMLPrompt" << "XMLRoom" << "XMLDescription"
		       << "XMLDynamicDescription" << "XMLExits"

		       << "XMLTerrain" << "XMLMagic" << "XMLWeather"

		       << "XMLTell" << "XMLSay" << "XMLNarrate" << "XMLSong"
		       << "XMLPray" << "XMLShout" << "XMLYell" << "XMLEmote"
		       << "XMLCommunication"
      
		       << "XMLHit" << "XMLDamage" << "XMLCombat";
    
    _readingTag = false;
    _xmlMode = XML_NONE;
    _removeXmlTags = true;

    _quit = false;
}


MumeXML::~MumeXML() {
  _quit = true;
  _cond.wakeOne();
  wait();
  qDebug() << "* MumeXML thread quit";
}


void MumeXML::customEvent(QEvent* e) {
  QMutexLocker locker(&_mutex);
  if (e->type() == 10000)
    engineEvent(e);
  else if (e->type() == 10001) {
    MClientEvent* me = static_cast<MClientEvent*>(e);
    
    QStringList types = me->dataTypes();
    foreach(QString s, types) {
      if (s.startsWith("TelnetData")) {
	_eventQueue.enqueue(me->payload()->toByteArray());
	if(!isRunning())
	  start(LowPriority);
	else
	  _cond.wakeOne();
      }
    }
  }
  else {
    qDebug() << "MumeXML somehow received the wrong kind of event...";

  }
}


void MumeXML::configure() {
}


bool MumeXML::loadSettings() {
    return true;
}


bool MumeXML::saveSettings() const {
    return true;
}


bool MumeXML::startSession(QString) {
    return true;
}


bool MumeXML::stopSession(QString) {
  return true;
}

void MumeXML::parse(const QByteArray& line) {
  int index;
  for (index = 0; index < line.size(); index++) {
    if (_readingTag) {
      if (line.at(index) == '>') {
        // Parse line according to the element's tag
        if (!_tempTag.isEmpty())
          element( _tempTag );

        _tempTag.clear();

// 	if (!_tempCharacters.isEmpty()) {
// 	  qDebug() << "Removing tempCharacters due to tag closure!" << _tempCharacters;
// 	  _tempCharacters.clear();
// 	}

        _readingTag = false;
        continue;
      }
      _tempTag.append(line.at(index));
    }
    else
    {
      if (line.at(index) == '<') {
        //send characters
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


void MumeXML::run() {
  while (!_quit) {
    while (!_eventQueue.isEmpty())
      parse(_eventQueue.dequeue());
    _cond.wait(&_mutex);
  }
}

bool MumeXML::element(const QByteArray& line) {
  int length = line.length();
  switch (_xmlMode)
  {
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
	  else
	  if (line.startsWith("terrain")) _xmlMode = XML_TERRAIN;
	  break;
	case 'w':
	  if (line.startsWith("weather")) _xmlMode = XML_WEATHER;
	  break;
        case '/':
          if (line.startsWith("/xml")) {
	    qDebug() << "XML mode disabled on MUME";
	    /*
            emit setNormalMode();
            emit sendToUser((QByteArray)"[MMapper] Mode ---> NORMAL\n");
            emptyQueue();
	    */
          }
          break;
      };
      break;
    case XML_ROOM:
      if (length > 0)
        switch (line.at(0))
      {
        case 'n':
          if (line.startsWith("name")) {
            _xmlMode = XML_NAME;
            _singleBuffer = emptyString; // might be empty but valid room name
          }
          break;
        case 'd':
          if (line.startsWith("description")) {
            _xmlMode = XML_DESCRIPTION;
            _multiBuffer = emptyString; // might be empty but valid description
          }
          break;
        case '/':
          if (line.startsWith("/room")) {
	    _xmlMode = XML_NONE;

	    // Post Dynamic Description Event
	    QVariant* qv = new QVariant(_multiBuffer);
	    QStringList sl;
	    sl << "XMLDynamicDescription" << "XMLAll";
	    postSession(qv, sl);

	  }
          break;
      }
      break;
    case XML_NAME:
      if (line.startsWith("/name")) {
	_xmlMode = XML_ROOM;

	// Post Room Name Event
	QVariant* qv = new QVariant(_singleBuffer);
	QStringList sl;
	sl << "XMLName" << "XMLAll";
	postSession(qv, sl);

      }
      break;
    case XML_DESCRIPTION:
      if (length > 0)
        switch (line.at(0))
      {
        case '/': if (line.startsWith("/description")) {
	  _xmlMode = XML_ROOM;

	  // Post Static Room Description Event
	  QVariant* qv = new QVariant(_multiBuffer);
	  QStringList sl;
	  sl << "XMLDescription" << "XMLAll";
	  postSession(qv, sl);

	  _multiBuffer = emptyString; // reset buffer for dynamic desc
	}
        break;
      }
      break;
    case XML_EXITS:
      if (length > 0)
        switch (line.at(0))
      {
        case '/': if (line.startsWith("/exits")) {
	  _xmlMode = XML_NONE;
	  
	  // Post Room Exits Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLExits" << "XMLAll";
	  postSession(qv, sl);

	}
        break;
      }
      break;
    case XML_PROMPT:
      if (length > 0)
        switch (line.at(0))
      {
        case '/': if (line.startsWith("/prompt")) {
	  _xmlMode = XML_NONE;

	  // Post Prompt Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLPrompt" << "XMLAll";
	  postSession(qv, sl);

	}
        break;
      }
      break;
    case XML_TERRAIN:
      if (length > 0)
	switch (line.at(0)) {
  	case '/': if (line.startsWith("/terrain")) {
	  _xmlMode = XML_NONE;

	  // Post Terrain Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLTerrain" << "XMLAll";
	  postSession(qv, sl);

	}
        break;
      }
    case XML_MAGIC:
      if (length > 0)
	switch (line.at(0)) {
  	case '/': if (line.startsWith("/magic")) {
	  _xmlMode = XML_NONE;

	  // Post Magic Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLMagic" << "XMLAll";
	  postSession(qv, sl);

	}
        break;
      }
    case XML_TELL:
      if (length > 0)
	switch (line.at(0)) {
  	case '/': if (line.startsWith("/tell")) {
	  _xmlMode = XML_NONE;

	  // Post Tell Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLTell" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);

	}
        break;
      }
    case XML_SAY:
      if (length > 0)
	switch (line.at(0)) {
  	case '/': if (line.startsWith("/say")) {
	  _xmlMode = XML_NONE;

	  // Post Say Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLSay" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);

	}
        break;
      }
    case XML_NARRATE:
      if (length > 0)
	switch (line.at(0)) {
  	case '/': if (line.startsWith("/narrate")) {
	  _xmlMode = XML_NONE;

	  // Post Narrate Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLNarrate" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);

	}
        break;
      }
    case XML_SONG:
      if (length > 0)
	switch (line.at(0)) {
  	case '/': if (line.startsWith("/song")) {
	  _xmlMode = XML_NONE;

	  // Post Song Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLSong" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);

	}
        break;
      }
    case XML_PRAY:
      if (length > 0)
	switch (line.at(0)) {
  	case '/': if (line.startsWith("/pray")) {
	  _xmlMode = XML_NONE;

	  // Post Pray Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLPray" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);

	}
        break;
      }
    case XML_SHOUT:
      if (length > 0)
	switch (line.at(0)) {
  	case '/': if (line.startsWith("/shout")) {
	  _xmlMode = XML_NONE;

	  // Post Shout Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLShout" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);

	}
        break;
      }
    case XML_YELL:
      if (length > 0)
	switch (line.at(0)) {
  	case '/': if (line.startsWith("/yell")) {
	  _xmlMode = XML_NONE;

	  // Post Yell Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLYell" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);

	}
        break;
      }
    case XML_EMOTE:
      if (length > 0)
	switch (line.at(0)) {
  	case '/': if (line.startsWith("/emote")) {
	  _xmlMode = XML_NONE;

	  // Post Emote Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLEmote" << "XMLAll" << "XMLCommunication";
	  postSession(qv, sl);

	}
        break;
      }
    case XML_DAMAGE:
      if (length > 0)
	switch (line.at(0)) {
  	case '/': if (line.startsWith("/damage")) {
	  _xmlMode = XML_NONE;

	  // Post Damage Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLDamage" << "XMLAll" << "XMLCombat";
	  postSession(qv, sl);

	}
        break;
      }
    case XML_HIT:
      if (length > 0)
	switch (line.at(0)) {
  	case '/': if (line.startsWith("/hit")) {
	  _xmlMode = XML_NONE;

	  // Post Hit Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLHit" << "XMLAll" << "XMLCombat";
	  postSession(qv, sl);

	}
        break;
      }
    case XML_WEATHER:
      if (length > 0)
	switch (line.at(0)) {
  	case '/': if (line.startsWith("/weather")) {
	  _xmlMode = XML_NONE;

	  // Post Weather Event
	  QVariant* qv = new QVariant(_singleBuffer);
	  QStringList sl;
	  sl << "XMLWeather" << "XMLAll";
	  postSession(qv, sl);
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

bool MumeXML::characters(QByteArray& ch) {
  // replace > and < chars
  ch.replace(greaterThanTemplate, greaterThanChar);
  ch.replace(lessThanTemplate, lessThanChar);
  
  /*
  _stringBuffer = QString::fromAscii(ch.constData(), ch.size());
  _stringBuffer = _stringBuffer.simplified();
  */
  QVariant* qv;
  QStringList sl;

  switch (_xmlMode) {
    case XML_NONE:        //non room info
      /*
      if (_readingRoomDesc == true) {
	_readingRoomDesc = false; // we finished read desc mode
	_descriptionReady = true;
	if (Config()._emulatedExits)
	  emulateExits();

 	// Empty QVariant for Exit Position Holder.
	qv = new QVariant(nullString); 
	sl << "XMLExits";
	postSession(qv, sl);

      } else {
        parseMudCommands(_stringBuffer);	
      }
      */
      //emit sendToUser(ch);
      qv = new QVariant(ch);
      sl << "XMLNone" << "XMLAll";
      postSession(qv, sl);
      break;
      
    case XML_ROOM: // dynamic line
      //_dynamicRoomDesc += _stringBuffer+"\n";
      _multiBuffer += ch;
      //emit sendToUser(ch);
      break;
    
    case XML_NAME:
      /*
      if  (_descriptionReady)
      {
        move();
      }
      */

      //removeAnsiMarks(_stringBuffer); //Remove room color marks

      //_readingRoomDesc = true; //start of read desc mode
      _singleBuffer = ch;
      _multiBuffer.clear();
      /*
      _descriptionReady = false;
      _roomName=_stringBuffer;
      _dynamicRoomDesc=nullString;
      _staticRoomDesc=nullString;
      _roomDescLines = 0;
      _readingStaticDescLines = true;
      _exits = nullString;
      */

      //emit sendToUser(ch);
      break;

      case XML_DESCRIPTION: // static line
        //removeAnsiMarks(_stringBuffer) ; //remove color marks
        //_staticRoomDesc += _stringBuffer+"\n";
	_multiBuffer += ch;
	/*
	if (_examine || !Config()._brief)
	emit sendToUser(ch);
	*/
        break;
    case XML_EXITS:
      //parseExits(_stringBuffer); //parse exits
      _singleBuffer = ch;

      /*
      if (_readingRoomDesc)
      {
        _readingRoomDesc = false;
        //_descriptionReady = true;
      }
      */
      //emit sendToUser(ch);
      break;

    case XML_PROMPT:
      /*
      if  (_descriptionReady)
	{
        if (_examine) _examine = false; // stop bypassing brief-mode

        parsePrompt(_stringBuffer);
        move();

      }
      else
      {
        if (!queue.isEmpty())
        {
          queue.dequeue();
          emit showPath(queue, true);
        }
      }
      */

      //ch = ch.trimmed(); // TODO: Why trim it!?
      //emit sendToUser(ch);
      _singleBuffer = ch;
      break;

    default:
      // This is for single-lined XML tags.
      _singleBuffer = ch;
      break;
  }

  return true;
}
