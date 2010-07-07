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

#ifndef _QUICKCONNECTDIALOG_H
#define _QUICKCONNECTDIALOG_H

#include <QDialog>
#include <QPointer>
#include "ui_QuickConnectDialog.h"

class ConfigManager;
class ProfileManagerDialog;

class QuickConnectDialog : public QDialog, public Ui::QuickConnectDialog
{
  Q_OBJECT

  public:
    QuickConnectDialog(ConfigManager *mgr, QWidget* parent=0);
    virtual ~QuickConnectDialog();

  signals:
    void profileSelected(const QString&);

  private slots:
    void playClicked();
    void profileClicked();
    void relayLoadProfile(const QString&);
    void doubleClicked(const QModelIndex &index);
    void selectionChanged(const QItemSelection &index);

   private:
    QString selectedProfile();

    ConfigManager *_mgr;
    QPointer<ProfileManagerDialog> _dialog;

};

#endif /* _QUICKCONNECTDIALOG_H */
