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

#ifndef PRESPAMMEDPATH_H_
#define PRESPAMMEDPATH_H_

#include <QtCore>
#include "parser.h"

class MapData;
class MapCanvas;

class PrespammedPath : public QObject
{
Q_OBJECT
public:
    PrespammedPath(QObject *parent = 0);
    ~PrespammedPath();
    
    bool isEmpty(){ return m_queue.isEmpty(); };    
    CommandQueue::const_iterator begin() const {return m_queue.begin();}
    CommandQueue::const_iterator end() const {return m_queue.end();}
    CommandQueue& getQueue() { return m_queue; };
signals:
	void update();
public slots:
    void setPath(CommandQueue, bool);
        
protected:

private:
	CommandQueue m_queue;
};

#endif
