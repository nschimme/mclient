#include "MMapperPluginParser.h"

#include "MapperManager.h"

#include "mapdata.h"
#include "mmapper2pathmachine.h"
#include "prespammedpath.h"

#include "patterns.h" // fog, etc

MMapperPluginParser::MMapperPluginParser(QString s, MapperManager *mapMgr,
					 QObject *parent)
  : AbstractParser(mapMgr->getMapData(), parent) {
  _session = s;

  connect(this, SIGNAL(event(ParseEvent* )),
          mapMgr->getPathMachine(),
	  SLOT(event(ParseEvent* )), Qt::QueuedConnection);
  
  connect(this, SIGNAL(releaseAllPaths()),
          mapMgr->getPathMachine(),
	  SLOT(releaseAllPaths()), Qt::QueuedConnection);
  
  connect(this, SIGNAL(showPath(CommandQueue, bool)),
          mapMgr->getPrespammedPath(),
	  SLOT(setPath(CommandQueue, bool)), Qt::QueuedConnection);

  _move = CID_NONE;
}


MMapperPluginParser::~MMapperPluginParser() {
}


void MMapperPluginParser::name(QString text, const QString &session) {
  if (session != _session) return ;
  removeAnsiMarks(text);
  m_roomName = text.simplified();
}


void MMapperPluginParser::description(const QString &text,
				      const QString &session) {
  if (session != _session) return ;

  if (m_descriptionReady) submit();

  m_staticRoomDesc = text.simplified();
  m_descriptionReady = true;
}


void MMapperPluginParser::dynamicDescription(const QString &text,
					     const QString &session) {
  if (session != _session) return ;
  m_dynamicRoomDesc = text.simplified();
}


void MMapperPluginParser::exits(QString text, const QString &session) {
  if (session != _session) return ;
  parseExits(text);
}


void MMapperPluginParser::prompt(QString text, const QString &session) {
  if (session != _session) return ;
  parsePrompt(text);
  submit();
}


void MMapperPluginParser::move(QString text, const QString &session) {
  if (session != _session) return ;

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

  qDebug() << "* MMapperPluginParser detected the character moved"
	   << m_roomName << m_staticRoomDesc << m_dynamicRoomDesc << _move;
}


void MMapperPluginParser::userInput(const QString &text,
				    const QString &session) {
  if (session != _session) return ;
  
}


void MMapperPluginParser::mudOutput(const QString &text,
				    const QString &session) {
  if (session != _session) return ;

  if (text.startsWith('Y')) {
    qDebug() << "* MMapperPluginParser detected some text" << text;

    if (text.startsWith("You are dead!")) {
      queue.clear();
      emit showPath(queue, true);
      emit releaseAllPaths();
      return;
    }
    else if (text.startsWith("You failed to climb")) {
      if(!queue.isEmpty()) queue.dequeue();
      queue.prepend(CID_NONE);
      emit showPath(queue, true);
      return;
    }
    else if (text.startsWith("You flee head")) {
      queue.enqueue(_move);
    }
    else if (text.startsWith("You follow")) {
      queue.enqueue(_move);
      return;
    }
    else if (text.startsWith("You quietly scout")) {
      queue.prepend(CID_SCOUT);
      return;
    }
  }
}
