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
#include <QInputDialog>
#include <QDebug>

//#include "ProfileEditDialog.h"
#include "ProfileManagerDialog.h"
#include "ConfigManager.h"

QString ProfileManagerDialog::selectedProfile ()
{
  QItemSelection sel = profileView->selectionModel()->selection();
  if (sel.empty()) return QString();
  int idx = sel.indexes().first().row();
  return _mgr->profileNames()[idx];
}

void ProfileManagerDialog::addClicked ()
{
  qDebug() << "! Add not implementd";
  /*
  //so first we have to create the dialog...
  mdlg = new ProfileEditDialog(this);

  //then we connect() all its signals - this handles everything that the dialog offers...
  connect (mdlg, SIGNAL (loadClicked ()), this, SLOT (doAdd ()));

  //dialog is ready - show it!
  mdlg->exec ();
  delete mdlg;
  */
}

void ProfileManagerDialog::modifyClicked ()
{
  qDebug() << "! Modify not implementd (or needed?)";

  /*
  cProfileManager *mgr = cProfileManager::self();
  QString profile = selectedProfile();
  cProfileSettings *sett = mgr->settings (profile);
  if (!sett) return;   // no profile selected

  //so first we have to create the dialog...
  mdlg = new ProfileEditDialog(this);

  //then we connect() all its signals - this handles everything that the dialog offers...
  connect (mdlg, SIGNAL (loadClicked ()), this, SLOT (doModify ()));

  mdlg->setName (mgr->visibleProfileName (profile));
  mdlg->setServer (sett->getString ("server"));
  mdlg->setPort (sett->getInt ("port"));
  mdlg->setDefinitions (sett->getString ("definitions"));
  mdlg->setMap (sett->getString ("map"));

  //dialog is ready - show it!
  mdlg->exec ();

  delete mdlg;
  */
}

void ProfileManagerDialog::updateProfileFromDialog (const QString &/*profile*/)
{
  /*
  cProfileManager *mgr = cProfileManager::self();
  cProfileSettings *sett = mgr->settings (profile);
  if (!sett) return;
  sett->setString ("server", mdlg->server());
  sett->setInt ("port", mdlg->port());
  sett->setString("definitions", mdlg->definitions());
  sett->setString("map", mdlg->map());
  sett->save();

  mgr->profileInfoChanged (profile);
  */
}

void ProfileManagerDialog::deleteClicked ()
{
  QString profile = selectedProfile();

  // can we do that ?
  if (1 /*_mgr->hasSessionAssigned (profile)*/) {
    QMessageBox::information(this,
                            tr("Unable to delete"),
                            tr("This profile can not be deleted, because you have a"
                               "connection open using this profile.")
                            );
    return;
  }

  if (QMessageBox::warning (this, tr("Delete profile"),
      tr("Do you really want to delete profile %1?").arg(profile),
         QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
    return;

  // wants to delete
  // TODO: offer the option to also delete the files
  //_mgr->deleteProfile (profile, false);
}

void ProfileManagerDialog::duplicateClicked ()
{
  QString profile = selectedProfile();
  
  bool ok;
  QString newName = QInputDialog::getText(this,
                                          tr("Duplicate Profile"),
                                          tr("Please enter name for the duplicated profile"),
                                          QLineEdit::Normal,
                                          profile,
                                          &ok
                                         );
  if (newName.isEmpty())
    return;

  if (!_mgr->duplicateProfile(profile, newName))
    QMessageBox::information(this,
			     tr("Unable to duplicate"),
			     tr("There was an error trying to duplicate the profile. "
				"Please ensure that you have write access to the profile directory.")
			     );
}

void ProfileManagerDialog::doAdd ()
{
  /*
  QString profile = _mgr->newProfile (mdlg->name());
  updateProfileFromDialog (profile);
  */
}

void ProfileManagerDialog::doModify ()
{
  /*
  QString profile = selectedProfile();
  _mgr->renameProfile(profile, mdlg->name());
  updateProfileFromDialog (profile);
  */
}

void ProfileManagerDialog::selectionChanged (const QItemSelection &index)
{
  //enable/disable Connect button
  loadButton->setEnabled(!index.indexes().empty());
}

void ProfileManagerDialog::doubleClicked (const QModelIndex &index)
{
  if (index.isValid ())
    loadButton->click();
}

void ProfileManagerDialog::loadClicked() {
  close();
  emit loadProfile(selectedProfile());
}

/* Objects */

ProfileManagerDialog::ProfileManagerDialog(ConfigManager *mgr, QItemSelectionModel *sel, QWidget*parent) : QDialog(parent), _mgr(mgr)
{
  setupUi(this);

  profileView->setModel(_mgr->model());
  profileView->setUniformRowHeights(true);
  profileView->setRootIsDecorated(false);
  profileView->setItemsExpandable(false);
  profileView->setWhatsThis(tr("This list shows currently defined profiles.<p><b>Profiles</b> "
                               "allow you to speed up connecting to your MUD, as well as the loading of"
                               "more advanced features like <i>aliases</i> or <i>triggers</i>.")
                           );

  loadButton = new QPushButton(tr("Load Profile"));
  loadButton->setDefault(true);
  buttonBox->addButton(loadButton, QDialogButtonBox::AcceptRole);

  // Synchronize Selection Models if it was called from QuickConnectDialog
  if (sel) {
    profileView->setSelectionModel(sel);
    selectionChanged(profileView->selectionModel()->selection());
  }
  /*
  else
    loadButton->hide();
  */

  connect (addButton, SIGNAL(clicked()), this, SLOT(addClicked()));
  connect (modifyButton, SIGNAL(clicked()), this, SLOT(modifyClicked()));
  connect (deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
  connect (duplicateButton, SIGNAL(clicked()), this, SLOT(duplicateClicked()));
  connect (loadButton, SIGNAL(clicked()), this, SLOT(loadClicked()));

  connect (profileView->selectionModel(), SIGNAL(selectionChanged (const QItemSelection &, const QItemSelection &)),
           this, SLOT(selectionChanged (const QItemSelection &)));
  connect (profileView, SIGNAL(doubleClicked (const QModelIndex &)),
           this, SLOT(doubleClicked (const QModelIndex &)));
}

ProfileManagerDialog::~ProfileManagerDialog() {}

