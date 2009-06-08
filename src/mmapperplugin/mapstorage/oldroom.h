/************************************************************************
**
** Authors:   Ulf Hermann <ulfonk_mennhar@gmx.de> (Alve), 
**            Marek Krejza <krejza@gmail.com> (Caligor)
**
** This file is part of the MMapper2 project. 
** Maintained by Marek Krejza <krejza@gmail.com>
**
** Copyright: See COPYING file that comes with this distribution
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
*************************************************************************/

#ifndef OLDROOM
#define OLDROOM
#include "room.h"
#include "oldconnection.h"
#include "mapdata.h"



#define RSF_ROAD_N    	bit1 //road
#define RSF_ROAD_S    	bit2
#define RSF_ROAD_E    	bit3
#define RSF_ROAD_W    	bit4
#define RSF_ROAD_U    	bit5
#define RSF_ROAD_D    	bit6
#define RSF_EXIT_N 		bit7 //exits WITHOUT doors
#define RSF_EXIT_S 		bit8
#define RSF_EXIT_E 		bit9
#define RSF_EXIT_W 		bit10
#define RSF_EXIT_U 		bit11
#define RSF_EXIT_D 		bit12
#define RSF_DOOR_N 		bit13 //exits WITH doors
#define RSF_DOOR_S 		bit14
#define RSF_DOOR_E 		bit15
#define RSF_DOOR_W 		bit16
#define RSF_DOOR_U 		bit17
#define RSF_DOOR_D 		bit18
#define RSF_NO_MATCH_N      bit19 //do not match exit [dir]
#define RSF_NO_MATCH_S      bit20
#define RSF_NO_MATCH_E      bit21
#define RSF_NO_MATCH_W      bit22
#define RSF_NO_MATCH_U      bit23
#define RSF_NO_MATCH_D      bit24
#define RSF_RESERVED_1      bit25
#define RSF_RESERVED_2      bit26
typedef quint32 RoomSpecialFlags;

typedef quint32 RoomID;
typedef QDateTime RoomTimeStamp;


typedef QList<Connection *> ConnectionList;
typedef QListIterator<Connection *> ConnectionListIterator;

#endif
