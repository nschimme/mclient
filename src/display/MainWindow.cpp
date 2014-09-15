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

#include <QtWidgets>
#include <QCloseEvent>
#include <QTabWidget>

#include "MainWindow.h"
#include "Session.h"
#include "WindowActionManager.h"
#include "ConfigManager.h"

#include "QuickConnectDialog.h"
#include "ProfileManagerDialog.h"
//#include "AliasEditorDialog.h"

MainWindow::MainWindow(ConfigManager* cfg) :
    QMainWindow(), _cfg(cfg) {
    setWindowIcon(QIcon(":/mainwindow/m.png"));
    readSettings();

    /** Create Other Child Widgets */
    // Initialize WindowActionManager
    _actMgr = new WindowActionManager(this);
    _actMgr->createActions();
    _actMgr->createMenus(menuBar());
    _actMgr->createToolBars();
    _actMgr->createStatusBar();

    /** Create Primary Display Widgets */
    _tabWidget = new QTabWidget();
    setCentralWidget(_tabWidget);

    qDebug() << "MainWindow created with thread:" << this->thread();
}


MainWindow::~MainWindow() {
    qDebug() << "* MainWindow got a quit!";
    //delete menuBar();
    qDebug() << "* MainWindow destroyed";
}

void MainWindow::startProfile(const QString &profile) {
    qDebug() << "* starting profile" << profile;

    setCurrentProfile(profile);

    Session *session = new Session(this);
    _tabWidget->addTab(session, profile);

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "MainWindow received closeEvent";
    if (maybeSave()) {
        writeSettings();
        //deleteLater();
        //getPluginManager()->deleteLater();
        //getPluginManager()->getConfig()->~ConfigManager();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::readSettings()
{
    resize(QSize(640, 480));
    //move(QPoint(200, 200));
}

void MainWindow::writeSettings()
{
    _cfg->writeApplicationSettings();
    /*
  Config().setWindowPosition(pos() );
  Config().setWindowSize(size() );
  Config().setWindowState(saveState() );
  Config().setAlwaysOnTop((bool)(windowFlags() &
  Qt::WindowStaysOnTopHint));
  */
}

bool MainWindow::maybeSave()
{
    /*
  if (Config().isChanged()) {
    int ret = QMessageBox::warning(this, tr("mClient"),
                                   tr("Your client settings have been modified.\n"
                                       "Do you want to save your changes?"),
                                       QMessageBox::Yes | QMessageBox::Default,
                                       QMessageBox::No,
                                       QMessageBox::Cancel | QMessageBox::Escape);
    if (ret == QMessageBox::Yes)
      Config().write();
    else if (ret == QMessageBox::Cancel)
      return false;
  }
  */
    return true;
}

void MainWindow::setCurrentProfile(const QString &session)
{
    _currentProfile = session;
    setWindowModified(false);

    QString shownName;
    if (_currentProfile.isEmpty())
        shownName = "No profile";
    else
        shownName = _currentProfile;

    setWindowTitle(tr("%1 - mClient").arg(shownName));
}

void MainWindow::manageProfiles() {
    ProfileManagerDialog *profileManager = new ProfileManagerDialog(_cfg, 0, this);

    // Connect the load signal
    connect(profileManager, SIGNAL(loadProfile(const QString&)),
            this, SLOT(startProfile(const QString&)));

    profileManager->exec();
}

void MainWindow::aliasEditor() {
    /*
  AliasEditorDialog aliasEditor
    = AliasEditorDialog(getPluginManager()->
                getPluginSession(_currentProfile)->
                getAlias(), this);
  aliasEditor.exec();
  */
}

const QString MainWindow::currentSession() const {
    return _tabWidget->tabText(_tabWidget->currentIndex());
}
