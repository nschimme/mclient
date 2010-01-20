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

#include "WindowActionManager.h"
#include "MainWindow.h"

#include "PluginSession.h"
#include "PluginManager.h"
#include "MClientEvent.h"
#include "SmartMenuBar.h"

WindowActionManager* WindowActionManager::_pinstance = 0;

WindowActionManager *WindowActionManager::instance(MainWindow *parent) {
  if(!_pinstance) {
    _pinstance = new WindowActionManager(parent);
    qDebug("Action Manager created.");
  }
  return _pinstance;
}


void WindowActionManager::destroy() {
  delete this;
}


WindowActionManager::WindowActionManager(MainWindow *parent) {
  _mainWindow = parent;
}


WindowActionManager::~WindowActionManager() {
}

void WindowActionManager::createActions() {
  exitAct = new QAction(/*QIcon(":/mainwindow/exit.png"),*/ tr("E&xit"), this);
  exitAct->setStatusTip(tr("Exit the application"));
  connect(exitAct, SIGNAL(triggered()), _mainWindow, SLOT(close()));

  cutAct = new QAction(QIcon(":/mainwindow/cut.png"), tr("Cu&t"), this);
  cutAct->setShortcut(tr("Ctrl+X"));
  cutAct->setStatusTip(tr("Cut the current selection's contents to the "
      "clipboard"));

  copyAct = new QAction(QIcon(":/mainwindow/copy.png"), tr("&Copy"), this);
  copyAct->setShortcut(tr("Ctrl+C"));
  copyAct->setStatusTip(tr("Copy the current selection's contents to the "
      "clipboard"));

  pasteAct = new QAction(QIcon(":/mainwindow/paste.png"), tr("&Paste"), this);
  pasteAct->setShortcut(tr("Ctrl+V"));
  pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
      "selection"));

  alwaysOnTopAct = new QAction(tr("&Always on Top"), this);
  alwaysOnTopAct->setCheckable(true);
  alwaysOnTopAct->setStatusTip(tr("Toggle the window to always stay on the top"));
  connect(alwaysOnTopAct, SIGNAL(triggered()), this, SLOT(alwaysOnTop()));

  fullScreenAct = new QAction(tr("&Full Screen"), this);
  fullScreenAct->setCheckable(true);
  fullScreenAct->setShortcut(tr("F11"));
  fullScreenAct->setStatusTip(tr("Toggle full screen mode"));
  connect(fullScreenAct, SIGNAL(triggered()), this, SLOT(fullScreen()));

profileAct = new QAction(QIcon(":/mainwindow/profile.png"), tr("Profile &Manager..."), this);
  profileAct->setStatusTip(tr("Manage mClient profile settings"));
  connect(profileAct, SIGNAL(triggered()), _mainWindow, SLOT(manageProfiles()));

  settingsAct = new QAction(QIcon(":/mainwindow/settings.png"), tr("&Preferences..."), this);
  settingsAct->setStatusTip(tr("Change mClient settings"));
  connect(settingsAct, SIGNAL(triggered()), _mainWindow, SLOT(changeConfiguration()) );

  aliasAct = new QAction(tr("&Alias"), this);
  aliasAct->setStatusTip(tr("Manage mClient aliases"));
  connect(aliasAct, SIGNAL(triggered()), _mainWindow, SLOT(aliasEditor()));


  mumeWebsiteAct = new QAction(tr("&Website"), this);
  mumeWebsiteAct->setStatusTip(tr("The official MUME website"));
  connect(mumeWebsiteAct, SIGNAL(triggered()), this, SLOT(openMumeWebsite()));
  mumeForumAct = new QAction(tr("&Forum"), this);
  mumeForumAct->setStatusTip(tr("Talk to other MUMErs"));
  connect(mumeForumAct, SIGNAL(triggered()), this, SLOT(openMumeForum()));
  mumeWikiAct = new QAction(tr("W&iki"), this);
  mumeWikiAct->setStatusTip(tr("View the MUME help files"));
  connect(mumeWikiAct, SIGNAL(triggered()), this, SLOT(openMumeWiki()));

  clientHelpAct = new QAction(QIcon(":/mainwindow/help.png"), tr("mClient &Help"), this);
  clientHelpAct->setStatusTip(tr("View the mClient help file"));
  connect(clientHelpAct, SIGNAL(triggered()), this, SLOT(clientHelp()));

  aboutAct = new QAction(QIcon(":/mainwindow/m.png"), tr("About &mClient"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAct = new QAction(QIcon(":/mainwindow/qt.png"), tr("About &Qt"), this);
  aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

  newbieHelpAct = new QAction(tr("&Information for Newcomers"), this);
  newbieHelpAct->setStatusTip("Newbie help on the MUME website");
  connect(newbieHelpAct, SIGNAL(triggered()), this, SLOT(newbieHelp()));

  cutAct->setEnabled(false);
  copyAct->setEnabled(false);
}


void WindowActionManager::disableActions(bool value)
{
//   newAct->setDisabled(value);
//   openAct->setDisabled(value);
//   mergeAct->setDisabled(value);
//   reloadAct->setDisabled(value);
//   saveAct->setDisabled(value);
//   saveAsAct->setDisabled(value);
  exitAct->setDisabled(value);
  cutAct->setDisabled(value);
  copyAct->setDisabled(value);
  pasteAct->setDisabled(value);
  aboutAct->setDisabled(value);
  aboutQtAct->setDisabled(value);
  alwaysOnTopAct->setDisabled(value);
  fullScreenAct->setDisabled(value);
}


void WindowActionManager::createMenus() {
  SmartMenuBar *menuBar = static_cast<SmartMenuBar*>(_mainWindow->menuBar());
  //QMenuBar *menuBar = _mainWindow->menuBar();

  fileMenu = menuBar->addMenu(tr("&File"));
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  editMenu = menuBar->addMenu(tr("&Edit"));
  editMenu->addAction(cutAct);
  editMenu->addAction(copyAct);
  editMenu->addAction(pasteAct);

  viewMenu = menuBar->addMenu(tr("&View"));
  viewMenu->addAction(alwaysOnTopAct);
  viewMenu->addAction(fullScreenAct);

  settingsMenu = menuBar->addMenu(tr("&Settings"));
  settingsMenu->addAction(settingsAct);
  settingsMenu->addAction(profileAct);
  settingsMenu->addAction(aliasAct);

  menuBar->addSeparator();

  helpMenu = menuBar->addMenu(tr("&Help"));
  helpMenu->addAction(clientHelpAct);
  helpMenu->addSeparator();
  newbieMenu = helpMenu->addMenu(tr("&Newbie Tutorials"));  
  newbieMenu->addAction(newbieHelpAct);
  helpMenu->addSeparator();
  mumeMenu = helpMenu->addMenu(tr("M&UME"));
  mumeMenu->addAction(mumeWebsiteAct);
  mumeMenu->addAction(mumeForumAct);
  mumeMenu->addAction(mumeWikiAct);
  helpMenu->addSeparator();
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);


}


void WindowActionManager::createToolBars() {
  /*
  editToolBar = _mainWindow->addToolBar(tr("Edit"));
  editToolBar->setObjectName("ToolBarEdit");
  editToolBar->addAction(cutAct);
  editToolBar->addAction(copyAct);
  editToolBar->addAction(pasteAct);
  editToolBar->setVisible(false);
  */
}


void WindowActionManager::createStatusBar()
{
  _mainWindow->statusBar()->showMessage(tr("Ready"));
}


void WindowActionManager::alwaysOnTop() {
  _mainWindow->setWindowFlags(_mainWindow->windowFlags() ^ Qt::WindowStaysOnTopHint);
  _mainWindow->show();
}


void WindowActionManager::fullScreen() {
  if (_mainWindow->isFullScreen()) {
    _mainWindow->showNormal();
  }
  else {
    _mainWindow->showFullScreen();
  }

  // Ensure alwaysOnTop is NOT set
  if (_mainWindow->windowFlags() & Qt::WindowStaysOnTopHint) {
    alwaysOnTop();
    alwaysOnTopAct->setChecked(false);
  }
}


void WindowActionManager::about() {
  QString version = 
#ifdef SVN_REVISION
    tr("<b>Subversion Revision ") + QString::number(SVN_REVISION)
#else
#ifdef MCLIENT_VERSION
    tr("<b>mClient ") + QString(MCLIENT_VERSION)
#else
    tr("<b>Unknown Release")
#endif
#endif
    + tr("</b><br><br>");
  QMessageBox::about(_mainWindow, tr("About mClient"),
                     tr("<FONT SIZE=\"+1\"><B>") + version +
		     tr("</B></FONT><P>"
			"Copyright \251 2008 by Jahara<P>"
			"Visit the <A HREF=\"http://code.google.com/p/mclient-mume/\">mClient website</A> "
			"to upgrade to the full mClient."));
}

void WindowActionManager::clientHelp() {
  if (!QDesktopServices::openUrl(QUrl::fromEncoded("http://mume.org/wiki/index.php/mClient_Help")))
    qWarning() << "Failed to open web browser";
}

void WindowActionManager::postEvent(QVariant *payload, const QStringList& tags) {
  // TODO: Replace with EngineEvents
  QString session = _mainWindow->currentSession();
  PluginSession *ps
    = _mainWindow->getPluginManager()->getPluginSession(session);
  
  MClientEvent *me
    = new MClientEvent(new MClientEventData(payload, tags, session));
  QCoreApplication::postEvent(ps, me);
}

void WindowActionManager::openMumeWebsite()
{
        QDesktopServices::openUrl(QUrl("http://mume.org/"));
}

void WindowActionManager::openMumeForum()
{
        QDesktopServices::openUrl(QUrl("http://mume.org/forum/"));
}

void WindowActionManager::openMumeWiki()
{
        QDesktopServices::openUrl(QUrl("http://mume.org/wiki/"));
}


void WindowActionManager::newbieHelp() {
  QDesktopServices::openUrl(QUrl("http://mume.org/newbie.php"));
}
