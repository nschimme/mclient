/***************************************************************************
 *   Copyright (C) 2008 by Nils Schimmelmann   *
 *   nschimme@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 ***************************************************************************/
//
// Author: Tomas Mecir <kmuddy@kmuddy.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QMessageBox>
#include <QDebug>

#include "QuickConnectDialog.h"
#include "ProfileManagerDialog.h"
#include "ConfigManager.h"

QString QuickConnectDialog::selectedProfile() {
  QItemSelection sel = profileView->selectionModel()->selection();
  if (sel.empty()) return QString();
  int idx = sel.indexes().first().row();
  return _mgr->profileNames()[idx];
}

void QuickConnectDialog::playClicked() {
  QString profile = selectedProfile();

  // Check if there is a loaded profile
  if (0) {
    if (QMessageBox::warning(this, tr("Load profile"),
			     tr("The profile %1 is already loaded.\nAre you sure you want to continue?").arg("TODO"),
			     QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
      return;
  }
  
  qDebug("running with settings...");
  accept();
  close();
  emit profileSelected(profile);
}

void QuickConnectDialog::relayLoadProfile(const QString& /*profile*/) {
  playClicked();
}

void QuickConnectDialog::profileClicked() {
  _dialog = new ProfileManagerDialog(_mgr, profileView->selectionModel(),
				     this);
   
  connect(_dialog, SIGNAL(loadProfile(const QString&)),
	  this, SLOT(relayLoadProfile(const QString&) ));

  _dialog->exec();

  delete _dialog;
}

void QuickConnectDialog::selectionChanged (const QItemSelection &index) {
  //enable/disable Connect button
  playButton->setEnabled(index.indexes().empty() ? false : true);
}

void QuickConnectDialog::doubleClicked(const QModelIndex &index) {
  // TODO: why does double clicking crash?!
  if (index.isValid())
    playClicked();
}

/* Objects */

QuickConnectDialog::QuickConnectDialog(ConfigManager *mgr, QWidget *parent)
  : QDialog(parent), _mgr(mgr)
{
  setupUi(this);

  QGraphicsScene *scene = new QGraphicsScene();
  scene->addPixmap(QPixmap(":/mainwindow/intro.png"));
  profilePicture->setScene(scene);
  profilePicture->show();

  playButton->setFocus();

  profileView->setModel(_mgr->model());
  profileView->setUniformRowHeights(true);
  profileView->setRootIsDecorated(false);
  profileView->setItemsExpandable(false);

  // Automatically select the last profile   TODO
  QModelIndex topLeft = profileView->model()->index(0, 0);
  QModelIndex bottomRight = profileView->model()->index(0, profileView->model()->columnCount()-1);
  QItemSelection selection(topLeft, bottomRight);
  profileView->selectionModel()->select(selection, QItemSelectionModel::Select);
  selectionChanged(profileView->selectionModel()->selection());


  connect(playButton, SIGNAL(clicked() ), this, SLOT(playClicked() ));
  connect(profileButton, SIGNAL(clicked() ), this, SLOT(profileClicked() ));

  connect(profileView->selectionModel(),
	  SIGNAL(selectionChanged(const QItemSelection&,
				  const QItemSelection&)),
	  this, SLOT(selectionChanged(const QItemSelection&)));
  connect(profileView, SIGNAL(doubleClicked(const QModelIndex&)),
          this, SLOT(doubleClicked(const QModelIndex&)));
}

QuickConnectDialog::~QuickConnectDialog() {}

