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

#include <QtGui>
#include <QTextStream>
#include <QCloseEvent>

#include "MainWindow.h"
#include "ActionManager.h"

#include "ConfigManager.h"
#include "PluginManager.h"

MainWindow* MainWindow::_pinstance = 0;

MainWindow* MainWindow::instance() {
    if(!_pinstance) {
        _pinstance = new MainWindow();
    }

    return _pinstance;
}

void MainWindow::destroy() {
    delete this;
}

MainWindow::MainWindow() {
  setWindowIcon(QIcon(":/mainwindow/m.png"));
  readSettings();

  /** Connect Other Necessary Widgets */
  connect(PluginManager::instance(), SIGNAL(doneLoading()), SLOT(start()));

  /** Create Other Child Widgets */
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  setCurrentProfile("");
  qDebug("MainWindow created.");
}

void MainWindow::start() {
  // Display Main Window
  show();

  PluginManager* pm = PluginManager::instance();
  pm->initSession("test");
}

void MainWindow::receiveWidget(const QWidget* widget) {
  // TODO: Different widget locations
  setCentralWidget((QWidget*)widget);  
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  writeSettings();
  if (maybeSave()) {
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::createActions() {
  ActionManager::self(this);
}

void MainWindow::createMenus() {
  ActionManager *actMgr = ActionManager::self();

  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(actMgr->connectAct);
  fileMenu->addAction(actMgr->disconnectAct);
  fileMenu->addAction(actMgr->reconnectAct);
  fileMenu->addSeparator();
  fileMenu->addAction(actMgr->exitAct);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(actMgr->cutAct);
  editMenu->addAction(actMgr->copyAct);
  editMenu->addAction(actMgr->pasteAct);

  viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(actMgr->alwaysOnTopAct);

  settingsMenu = menuBar()->addMenu(tr("&Settings"));
  settingsMenu->addAction(actMgr->settingsAct);

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(actMgr->mumeHelpAct);
  helpMenu->addAction(actMgr->forumAct);
  helpMenu->addAction(actMgr->wikiAct);
  helpMenu->addSeparator();
  helpMenu->addAction(actMgr->clientHelpAct);
  helpMenu->addSeparator();
  helpMenu->addAction(actMgr->aboutAct);
  helpMenu->addAction(actMgr->aboutQtAct);
}

void MainWindow::createToolBars() {
  ActionManager *actMgr = ActionManager::self();

  connectToolBar = addToolBar(tr("Connection"));
  connectToolBar->setObjectName("ToolBarConnect");
  connectToolBar->addAction(actMgr->connectAct);
  connectToolBar->addAction(actMgr->disconnectAct);
  connectToolBar->addAction(actMgr->reconnectAct);

  editToolBar = addToolBar(tr("Edit"));
  editToolBar->setObjectName("ToolBarEdit");
  editToolBar->addAction(actMgr->cutAct);
  editToolBar->addAction(actMgr->copyAct);
  editToolBar->addAction(actMgr->pasteAct);
}

void MainWindow::createStatusBar()
{
  statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
  resize(QSize(640, 480));
  move(QPoint(200, 200));
}

void MainWindow::writeSettings()
{
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
  return true;
  */
}

void MainWindow::setCurrentProfile(const QString &profile)
{
  currentProfile = profile;
  setWindowModified(false);

  QString shownName;
  if (currentProfile.isEmpty())
    shownName = "No connection";
  else
    shownName = currentProfile;

  setWindowTitle(tr("%1 - mClient").arg(shownName));
}

MainWindow::~MainWindow() {
  PluginManager* pm = PluginManager::instance();
  ConfigManager* cm = ConfigManager::instance();
  pm->destroy();
  cm->destroy();
  qDebug("MainWindow Destroyed");
}

void MainWindow::changeConfiguration() {
  PluginManager* pm = PluginManager::instance();
  pm->configure();
}

