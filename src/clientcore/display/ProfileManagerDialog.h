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

#ifndef _PROFILEMANAGERDIALOG_H
#define _PROFILEMANAGERDIALOG_H

#include <QDialog>
#include "ui_ProfileManagerDialog.h"

//class ProfileEditDialog;
class ConfigManager;

class ProfileManagerDialog : public QDialog, public Ui::ProfileManagerDialog
{
  Q_OBJECT

  public:
    ProfileManagerDialog(ConfigManager *mgr, QItemSelectionModel *model,
			 QWidget* parent);
    virtual ~ProfileManagerDialog();

    QString selectedProfile ();

  private:
    void updateProfileFromDialog(const QString &profile);
    //ProfileEditDialog *mdlg;
    QPushButton *loadButton;
    ConfigManager *_mgr;

  signals:
    void loadProfile(const QString &profile);

  public slots:
    void doAdd();
    void doModify();

  private slots:
    void addClicked();
    void modifyClicked();
    void deleteClicked();
    void duplicateClicked();
    void loadClicked();
    void doubleClicked(const QModelIndex &index);
    void selectionChanged(const QItemSelection &index);
    
};

#endif /* _PROFILEMANAGERDIALOG_H */
