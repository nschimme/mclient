#include "MMapperPluginParser.h"

#include "MMapperPlugin.h"
#include "MapperManager.h"

#include "mapdata.h"
#include "mmapper2pathmachine.h"
#include "prespammedpath.h"

#include "patterns.h" // fog, etc

MMapperPluginParser::MMapperPluginParser(MapperManager *mapMgr,
					 QObject *parent)
  : AbstractParser(mapMgr->getMapData(), parent) {

  _move = CID_NONE;

  qDebug() << "MMapperPluginParser loaded";
}


MMapperPluginParser::~MMapperPluginParser() {
}


void MMapperPluginParser::name(QString text) {
  removeAnsiMarks(text);
  m_roomName = text.trimmed();
}


void MMapperPluginParser::description(const QString &text) {
  m_staticRoomDesc = QString(text).remove(QChar('\r'));
  latinToAscii(m_staticRoomDesc);
}


void MMapperPluginParser::terrain(const QString &/*text*/) {
}


void MMapperPluginParser::dynamicDescription(const QString &text) {
  m_dynamicRoomDesc = text;
}


void MMapperPluginParser::exits(QString text) {
  m_descriptionReady = true;

  if (text.isEmpty()) {
    if (!queue.isEmpty())
      if (queue.head() == CID_SCOUT)
	return ;
    emulateExits();
  }
  else {
    parseExits(text);
  }
}


void MMapperPluginParser::prompt(QString text) {
  parsePrompt(text);

  if (m_descriptionReady) submit();
}


void MMapperPluginParser::move(const QString &text) {
  switch (text.at(0).toAscii()) {
    case 'n':
      _move = CID_NORTH;
      break;
    case 's':
      _move = CID_SOUTH;
      break;
    case 'e':
      _move = CID_EAST;
      break;
    case 'w':
      _move = CID_WEST;
      break;
    case 'u':
      switch (text.at(1).toAscii()) {
      case 'p':
	_move = CID_UP;
	break;
      case 'n':
      default:
	_move = CID_NONE;
	break;
      };
      break;
    case 'd':
      _move = CID_DOWN;
      break;
    default:
      _move = CID_NONE;
    };
}


void MMapperPluginParser::submit() {
  m_descriptionReady = false;
  
  // non standard end of description parsed (fog, dark or so ...)
  if (Patterns::matchNoDescriptionPatterns(m_roomName)) {
    qDebug() << "* fog, dark or so ..., nulling strings";
    m_roomName = nullString;
    m_dynamicRoomDesc = nullString;
    m_staticRoomDesc = nullString;
  }
  
  if (!queue.isEmpty()) {
    CommandIdType c = queue.dequeue();
    if ( c != CID_SCOUT ) {
      emit showPath(queue, false);
      characterMoved(_move, m_roomName, m_dynamicRoomDesc, m_staticRoomDesc, m_exitsFlags, m_promptFlags);
      if (c != _move)
        queue.clear();
      _move = CID_LOOK;
    }

  } else {
    //emit showPath(queue, false);
    characterMoved(_move, m_roomName, m_dynamicRoomDesc, m_staticRoomDesc, m_exitsFlags, m_promptFlags);
    _move = CID_LOOK;
  }

  m_exitsFlags = 0;
  m_promptFlags = 0;
  m_roomName = nullString;
  m_dynamicRoomDesc = nullString;
  m_staticRoomDesc = nullString;
}


void MMapperPluginParser::userInput(QString text) {
  qDebug() << "* MMapperPluginParser got input" << text;
  if (parseUserCommands(text))
    emit sendToMud(text.toAscii());
}


void MMapperPluginParser::mudOutput(const QString &text) {
  if (text.isEmpty()) return;

  if (text.startsWith('Y')) {
    qDebug() << "* MMapperPluginParser detected some text" << text;

    if (text.startsWith("You are dead!")) {
      queue.clear();
      emit showPath(queue, true);
      emit releaseAllPaths();
      return;
    }
    else if (text.startsWith("You failed to climb")) {
      if (!queue.isEmpty()) queue.dequeue();
      queue.prepend(CID_NONE);
      emit showPath(queue, true);
      return;
    }
    else if (text.startsWith("You are too exhausted.")) {
      if (!queue.isEmpty()) queue.dequeue();
      //queue.prepend(CID_NONE);
      emit showPath(queue, true);
      return;
    }
    /*
    else if (text.startsWith("You flee head")) {
      queue.enqueue(_move);
    }
    else if (text.startsWith("You follow")) {
      queue.enqueue(_move);
      return;
    }
    */
    else if (text.startsWith("You quietly scout")) {
      queue.prepend(CID_SCOUT);
      return;
    }
  }
}
