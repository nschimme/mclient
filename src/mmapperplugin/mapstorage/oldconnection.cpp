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

#include "oldconnection.h"

Connection::Connection() : 
  m_type(CT_NORMAL),
  m_flags(0),
  m_index(0)
{
  m_rooms[LEFT] = NULL;
  m_rooms[RIGHT] = NULL;
  m_doors[LEFT] = NULL;
  m_doors[RIGHT] = NULL;
  m_directions[LEFT] = CD_UNKNOWN;
  m_directions[RIGHT] = CD_UNKNOWN;
}

Connection::~ Connection()
{ // no one keeps track of the doors so we have to remove them here
  delete m_doors[LEFT];
  delete m_doors[RIGHT];
}

ConnectionDirection opposite(ConnectionDirection in)
{

  switch (in)
  {
  case CD_NORTH: return CD_SOUTH;
  case CD_SOUTH: return CD_NORTH;
  case CD_WEST: return CD_EAST;
  case CD_EAST: return CD_WEST;
  case CD_UP: return CD_DOWN;
  case CD_DOWN: return CD_UP;
  default: return CD_UNKNOWN;
  }

}

