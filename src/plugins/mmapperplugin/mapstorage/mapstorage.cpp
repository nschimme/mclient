/************************************************************************
**
** Authors:   Ulf Hermann <ulfonk_mennhar@gmx.de> (Alve),
**            Marek Krejza <krejza@gmail.com> (Caligor),
**            Nils Schimmelmann <nschimme@gmail.com> (Jahara)
**
** This file is part of the MMapper project. 
** Maintained by Nils Schimmelmann <nschimme@gmail.com>
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the:
** Free Software Foundation, Inc.
** 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
************************************************************************/

#include <cassert>
#include <iostream>
#include "mapstorage.h"
#include "mapdata.h"
#include "roomsaver.h"
#include "patterns.h"
#include "configuration.h"
#include "mmapper2room.h"
#include "mmapper2exit.h"
#include "progresscounter.h"
#include "basemapsavefilter.h"

using namespace std;

MapStorage::MapStorage(MapData& mapdata, const QString& filename, QFile* file) :
    AbstractMapStorage(mapdata, filename, file)
{}

MapStorage::MapStorage(MapData& mapdata, const QString& filename) :
    AbstractMapStorage(mapdata, filename)
{}

void MapStorage::newData()
{
  m_mapData.unsetDataChanged();

  m_mapData.setFileName(m_fileName);

  Coordinate pos;
  m_mapData.setPosition(pos);

  //clear previous map
  m_mapData.clear();
  emit onNewData();
}

Room * MapStorage::loadRoom(QDataStream & stream, qint32 version)
{
  QString vqba;
  quint32 vquint32;
  quint8 vquint8;
  quint16 vquint16;
  Room * room = factory.createRoom();
  room->setPermanent();

  stream >> vqba;
  room->replace(R_NAME, vqba);
  stream >> vqba;
  room->replace(R_DESC, vqba);
  stream >> vqba;
  room->replace(R_DYNAMICDESC, vqba);
  stream >> vquint32; room->setId(vquint32+baseId);
  stream >> vqba;
  room->replace(R_NOTE, vqba);
  stream >> vquint8;
  room->replace(R_TERRAINTYPE, vquint8);
  stream >> vquint8; room->replace(R_LIGHTTYPE, vquint8);
  stream >> vquint8; room->replace(R_ALIGNTYPE, vquint8);
  stream >> vquint8;
  room->replace(R_PORTABLETYPE, vquint8);  
  if (version >= 030)
	  stream >> vquint8;
  else
	  vquint8 = 0;
  room->replace(R_RIDABLETYPE, vquint8);
  stream >> vquint16; room->replace(R_MOBFLAGS, vquint16);
  stream >> vquint16; room->replace(R_LOADFLAGS, vquint16);

  stream >> vquint8; //roomUpdated
  if (vquint8)
  {
    room->setUpToDate();
  }
  Coordinate c;
  stream >> (qint32 &)c.x;
  stream >> (qint32 &)c.y;
  stream >> (qint32 &)c.z;
  
  room->setPosition(c + basePosition);
  loadExits(room, stream, version);
  return room;
}

void MapStorage::loadExits(Room * room, QDataStream & stream, qint32 /*version*/)
{
  ExitsList & eList = room->getExitsList();
  for (int i = 0; i < 7; ++i)
  {
    Exit & e = eList[i];

    ExitFlags flags;
    stream >> flags;
    if (ISSET(flags, EF_DOOR)) SET(flags, EF_EXIT);
    e[E_FLAGS] = flags;

    DoorFlags dFlags;
    stream >> dFlags;
    e[E_DOORFLAGS] = dFlags;

    DoorName dName;
    stream >> dName;
    e[E_DOORNAME] = dName;

    quint32 connection;
    for (stream >> connection; connection != UINT_MAX; stream >> connection)
    {
      e.addIn(connection+baseId);
    }
    for (stream >> connection; connection != UINT_MAX; stream >> connection)
    {
      e.addOut(connection+baseId);
    }
  }
}

bool MapStorage::loadData()
{
  //clear previous map
  m_mapData.clear();
  return mergeData();
}


bool MapStorage::mergeData()
{
  {
    MapFrontendBlocker blocker(m_mapData);
    
    baseId = m_mapData.getMaxId() + 1;
    basePosition = m_mapData.getLrb();
    if (basePosition.x + basePosition.y + basePosition.z != 0)
    {
	    //basePosition.y++;
	    //basePosition.x = 0;
	    //basePosition.z = 0;
	    basePosition.y = 0;
	    basePosition.x = 0;
	    basePosition.z = -1;
    }
    
    emit log ("MapStorage", "Loading data ...");
    m_progressCounter->reset();

    QDataStream stream (m_file);

    Room *room = NULL;
    InfoMark *mark = NULL;

    quint32 magic;
    qint32 version;
    quint32 index = 0;
    quint32 roomsCount = 0;
    quint32 marksCount = 0;

    m_mapData.setDataChanged();

    // Read the version and magic
    stream >> magic;
    if ( magic != 0xFFB2AF01 ) return false;
    stream >> version;
    if ( version != 031 && version != 030 &&
	 version <= 020 ) return false;

    // QtIOCompressor
    if (version >= 031) {
      m_compressor->open(QIODevice::ReadOnly);
      stream.setDevice(m_compressor);
    }

    stream >> roomsCount;
    stream >> marksCount;

    m_progressCounter->increaseTotalStepsBy( roomsCount + marksCount );

    Coordinate pos;

    //read selected room x,y
    stream >> (qint32 &)pos.x;
    stream >> (qint32 &)pos.y;
    stream >> (qint32 &)pos.z;

    pos += basePosition;
    
    m_mapData.setPosition(pos);

    emit log ("MapStorage", QString("Number of rooms: %1").arg(roomsCount) );

    RoomList roomList(roomsCount);
    for (uint i = 0; i < roomsCount; ++i)
    {
      room = loadRoom(stream, version);

      m_progressCounter->step();
      m_mapData.insertPredefinedRoom(room);
    }

    emit log ("MapStorage", QString("Number of info items: %1").arg(marksCount) );


    MarkerList& markerList = m_mapData.getMarkersList();
    // create all pointers to items
    for(index = 0; index<marksCount; index++)
    {
      mark = new InfoMark();
      loadMark(mark, stream, version);      
      markerList.append(mark);
      
      m_progressCounter->step();
    }

    emit log ("MapStorage", "Finished loading.");

    if (m_mapData.getRoomsCount() < 1 ) return false;

    m_mapData.setFileName(m_fileName);
    m_mapData.unsetDataChanged();

    if (version >= 031) m_compressor->close();

  }

  m_mapData.checkSize();
  emit onDataLoaded();
  return true;
}



void MapStorage::loadMark(InfoMark * mark, QDataStream & stream, qint32 /*version*/)
{
  QString vqstr;
  qint32  vqint32;

  qint32 postfix = basePosition.x + basePosition.y + basePosition.z;

  QString vqba;
  QDateTime vdatetime;
  quint8 vquint8;

  stream >> vqba; 
  if (postfix != 0 && postfix != 1)
  {
    vqba += QString("_m%1").arg(postfix).toAscii();
  }   
  mark->setName(vqba);
  stream >> vqba; mark->setText(vqba);
  stream >> vdatetime; mark->setTimeStamp(vdatetime);
  stream >> vquint8; mark->setType((InfoMarkType)vquint8);

  Coordinate pos;
  stream >> vqint32; pos.x = vqint32/*-40*/;
  stream >> vqint32; pos.y = vqint32/*-55*/;
  stream >> vqint32; pos.z = vqint32;
  pos.x += basePosition.x*100;
  pos.y += basePosition.y*100;
  pos.z += basePosition.z;
  //pos += basePosition;
  mark->setPosition1(pos);

  stream >> vqint32; pos.x = vqint32/*-40*/;
  stream >> vqint32; pos.y = vqint32/*-55*/;
  stream >> vqint32; pos.z = vqint32;
  pos.x += basePosition.x*100;
  pos.y += basePosition.y*100;
  pos.z += basePosition.z;
  //pos += basePosition;
  mark->setPosition2(pos);
}

void MapStorage::saveRoom(const Room * room, QDataStream & stream)
{
  stream << getName(room);
  stream << getDescription(room);
  stream << getDynamicDescription(room);
  stream << (quint32)room->getId();
  stream << getNote(room);
  stream << (quint8)getTerrainType(room);
  stream << (quint8)getLightType(room);
  stream << (quint8)getAlignType(room);
  stream << (quint8)getPortableType(room);
  stream << (quint8)getRidableType(room);
  stream << (quint16)getMobFlags(room);
  stream << (quint16)getLoadFlags(room);

  stream << (quint8)room->isUpToDate();

  const Coordinate & pos = room->getPosition();
  stream << (qint32)pos.x;
  stream << (qint32)pos.y;
  stream << (qint32)pos.z;
  saveExits(room, stream);
}

void MapStorage::saveExits(const Room * room, QDataStream & stream)
{
  const ExitsList& exitList = room->getExitsList();
  ExitsListIterator el(exitList);
  while (el.hasNext())
  {
    const Exit & e = el.next();
    stream << getFlags(e);
    stream << getDoorFlags(e);
    stream << getDoorName(e);
    for (set<uint>::const_iterator i = e.inBegin(); i != e.inEnd(); ++i)
    {
      stream << (quint32)*i;
    }
    stream << (quint32)UINT_MAX;
    for (set<uint>::const_iterator i = e.outBegin(); i != e.outEnd(); ++i)
    {
      stream << (quint32)*i;
    }
    stream << (quint32)UINT_MAX;
  }
}

bool MapStorage::saveData( bool baseMapOnly )
{
  emit log ("MapStorage", "Writing data to file ...");

  QDataStream stream (m_file);

  // Collect the room and marker lists. The room list can't be acquired
  // directly apparently and we have to go through a RoomSaver which receives
  // them from a sort of callback function.
  QList<const Room *> roomList;
  MarkerList& markerList = m_mapData.getMarkersList();
  RoomSaver saver(&m_mapData, roomList);
  for (uint i = 0; i < m_mapData.getRoomsCount(); ++i)
  {
    m_mapData.lookingForRooms(&saver, i);
  }

  uint roomsCount = saver.getRoomsCount();
  uint marksCount = markerList.size();

  m_progressCounter->reset();
  m_progressCounter->increaseTotalStepsBy( roomsCount + marksCount );

  BaseMapSaveFilter filter;
  if ( baseMapOnly )
  {
      filter.setMapData( &m_mapData );
      m_progressCounter->increaseTotalStepsBy( filter.prepareCount() );
      filter.prepare( m_progressCounter );
      roomsCount = filter.acceptedRoomsCount();
  }

  // Write a header with a "magic number" and a version
  stream << (quint32)0xFFB2AF01;
  stream << (qint32)031;

  // QtIOCompressor
  m_compressor->open(QIODevice::WriteOnly);
  stream.setDevice(m_compressor);

  //write counters
  stream << (quint32)roomsCount;
  stream << (quint32)marksCount;

  //write selected room x,y
  Coordinate & self = m_mapData.getPosition();
  stream << (qint32)self.x;
  stream << (qint32)self.y;
  stream << (qint32)self.z;

  // save rooms
  QListIterator<const Room *>  roomit(roomList);
  while (roomit.hasNext())
  {
    const Room *room = roomit.next();
    if ( baseMapOnly )
    {
      BaseMapSaveFilter::Action action = filter.filter( room );
      if ( !room->isTemporary() && action != BaseMapSaveFilter::REJECT )
      {
        if ( action == BaseMapSaveFilter::ALTER )
        {
          Room copy = filter.alteredRoom( room );
          saveRoom( &copy, stream );
        }
        else // action == PASS
        {
          saveRoom(room, stream);
        }
      }
    }
    else
    {
      saveRoom(room, stream);
    }

    m_progressCounter->step();
  }

  // save items
  MarkerListIterator markerit(markerList);
  while (markerit.hasNext())
  {
    InfoMark *mark = markerit.next();
    saveMark(mark, stream);

    m_progressCounter->step();
  }
  
  m_compressor->close();
  
  emit log ("MapStorage", "Writting data finished.");

  m_mapData.unsetDataChanged();
  emit onDataSaved();

  return TRUE;
}


void MapStorage::saveMark(InfoMark * mark, QDataStream & stream)
{
  stream << (QString)mark->getName();
  stream << (QString)mark->getText();
  stream << (QDateTime)mark->getTimeStamp();
  stream << (quint8)mark->getType();
  Coordinate & c1 = mark->getPosition1();
  Coordinate & c2 = mark->getPosition2();
  stream << (qint32)c1.x;
  stream << (qint32)c1.y;
  stream << (qint32)c1.z;
  stream << (qint32)c2.x;
  stream << (qint32)c2.y;
  stream << (qint32)c2.z;
}

