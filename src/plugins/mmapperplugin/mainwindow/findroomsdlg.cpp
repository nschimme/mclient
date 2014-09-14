#include <QtGui>
#include <QCloseEvent>
#include "findroomsdlg.h"
//#include "mmapper2event.h" //createEvent
#include "mmapper2room.h"  //getName
#include "mmapper2exit.h"  //getExit

const QString FindRoomsDlg::nullString;

FindRoomsDlg::FindRoomsDlg(MapData* md, QWidget *parent)
  : QDialog(parent)
{
  setupUi(this);

  m_mapData = md;
  m_admin = NULL;
  adjustResultTable();

  connect(lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(enableFindButton(const QString &)));
  connect(findButton, SIGNAL(clicked()), this, SLOT(findClicked()));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(resultTable, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem *)));
}

void FindRoomsDlg::findClicked()
{
  // Release rooms for a new search
  if (m_admin != NULL) m_mapData->unselect(m_roomSelection);

  m_roomSelection = m_mapData->select();
  Qt::CaseSensitivity cs = caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
  QString text = lineEdit->text();
  resultTable->clear();
  roomsFoundLabel->clear();

  /*  for an absolute match do the below:
  m_mapData->lookingForRooms(this, createEvent(CID_UNKNOWN, text, nullString, nullString, 0, 0));
  */

  if (nameRadioButton->isChecked())
    m_mapData->searchNames(this, text, cs);
  else if (descRadioButton->isChecked())
    m_mapData->searchDescriptions(this, text, cs);
  else if (exitsRadioButton->isChecked())
    m_mapData->searchDoorNames(this, text, cs);
}

void FindRoomsDlg::receiveRoom(RoomAdmin * sender, const Room * room)
{
  m_admin = sender;
  m_mapData->select(m_mapData->select(room->getId()), m_roomSelection);

  QString id;
  id.setNum(room->getId());
  QString roomName = QString(getName(room));

  m_admin->releaseRoom(this, room->getId()); // When do I release rooms? Now? later?

  item = new QTreeWidgetItem(resultTable);
  item->setText(0, id);
  item->setText(1, roomName);
  roomsFoundLabel->setText(tr("%1 room(s) found").arg( resultTable->topLevelItemCount() ));
}

void FindRoomsDlg::itemDoubleClicked(QTreeWidgetItem *item)
{
  uint id;
  if (item == NULL)
    return;

  id = item->text(0).toInt();

  const Room *r = m_mapData->getRoom(id, m_roomSelection);
  Coordinate c = r->getPosition();
  emit center(c.x, c.y); // connects to MapWindow

  // taken from MapCanvas:
  QString etmp = "Exits:";
  for (int j = 0; j < 7; j++) {

    bool door = false;
    if (ISSET(getFlags(r->exit(j)),EF_DOOR))
    {
      door = true;
      etmp += " (";
    }

    if (ISSET(getFlags(r->exit(j)),EF_EXIT)) {
      if (!door) etmp += " ";

      switch(j)
      {
        case 0: etmp += "north"; break;
        case 1: etmp += "south"; break;
        case 2: etmp += "east"; break;
        case 3: etmp += "west"; break;
        case 4: etmp += "up"; break;
        case 5: etmp += "down"; break;
        case 6: etmp += "unknown"; break;
      }
    }

    if (door)
    {
      if (getDoorName(r->exit(j))!="")
        etmp += "/"+getDoorName(r->exit(j))+")";
      else
        etmp += ")";
    }
  }
  etmp += ".\n";
  QString idtemp = QString("Selected Room ID: %1").arg(id);
  emit log( "FindRooms", idtemp+"\n"+getName(r)+"\n"+getDescription(r)+getDynamicDescription(r)+etmp);
//  emit newRoomSelection(m_roomSelection);
}

void FindRoomsDlg::adjustResultTable()
{
  resultTable->setColumnCount(2);
  resultTable->setHeaderLabels(QStringList() << tr("Room ID") << tr("Room Name"));
  resultTable->header()->resizeSections(QHeaderView::Stretch);
  resultTable->setRootIsDecorated(false);
  resultTable->setAlternatingRowColors(true);
}

void FindRoomsDlg::enableFindButton(const QString &text)
{
  findButton->setEnabled(!text.isEmpty());
}

void FindRoomsDlg::closeEvent( QCloseEvent * event )
{
  // Release data back to the m_admin
  if (m_admin != NULL) m_mapData->unselect(m_roomSelection);
  resultTable->clear();
  roomsFoundLabel->clear();
  m_admin = NULL;
  event->accept();
}

void FindRoomsDlg::on_lineEdit_textChanged()
{
  findButton->setEnabled(lineEdit->hasAcceptableInput());
}
