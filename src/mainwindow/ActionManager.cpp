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

#include "ActionManager.h"
#include "MainWindow.h"

#include "PluginSession.h"
#include "PluginManager.h"
#include "MClientEvent.h"

ActionManager* ActionManager::_pinstance = 0;

ActionManager *ActionManager::instance(MainWindow *parent) {
  if(!_pinstance) {
    _pinstance = new ActionManager(parent);
    qDebug("Action Manager created.");
  }
  return _pinstance;
}


void ActionManager::destroy() {
  delete this;
}


ActionManager::ActionManager(MainWindow *parent) {
  _mainWindow = parent;
}


ActionManager::~ActionManager() {
}

void ActionManager::createActions() {
  connectAct = new QAction(QIcon(":/mainwindow/connect.png"), tr("&Connect..."), this);
  connectAct->setStatusTip(tr("Load a new session and connect to the remote host"));
  connect(connectAct, SIGNAL(triggered()), SLOT(connectSession()) );
  
  disconnectAct = new QAction(QIcon(":/mainwindow/disconnect.png"), tr("&Disconnect"), this);
  disconnectAct->setStatusTip(tr("Disconnect from the current session"));
  connect(disconnectAct, SIGNAL(triggered()), SLOT(disconnectSession()) );

  reconnectAct = new QAction(QIcon(":/mainwindow/reconnect.png"), tr("&Reconnect"), this);
  reconnectAct->setStatusTip(tr("Reconnect to the current session's remote host"));
  connect(reconnectAct, SIGNAL(triggered()), SLOT(reconnectSession()) );

  exitAct = new QAction(QIcon(":/mainwindow/exit.png"), tr("E&xit"), this);
  exitAct->setStatusTip(tr("Exit the application"));
  connect(exitAct, SIGNAL(triggered()), _mainWindow, SLOT(close()));

  cutAct = new QAction(QIcon(":/mainwindow/cut.png"), tr("Cu&t"), this);
  cutAct->setShortcut(tr("Ctrl+X"));
  cutAct->setStatusTip(tr("Cut the current selection's contents to the "
      "clipboard"));
  //connect(cutAct, SIGNAL(triggered()), cliMgr->getTextEdit(), SLOT(cut()));

  copyAct = new QAction(QIcon(":/mainwindow/copy.png"), tr("&Copy"), this);
  copyAct->setShortcut(tr("Ctrl+C"));
  copyAct->setStatusTip(tr("Copy the current selection's contents to the "
      "clipboard"));
  //connect(copyAct, SIGNAL(triggered()), cliMgr->getTextEdit(), SLOT(copy()));

  pasteAct = new QAction(QIcon(":/mainwindow/paste.png"), tr("&Paste"), this);
  pasteAct->setShortcut(tr("Ctrl+V"));
  pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
      "selection"));
  //connect(pasteAct, SIGNAL(triggered()), cliMgr->getLineEdit(), SLOT(paste()));

  alwaysOnTopAct = new QAction(tr("&Always on Top"), this);
  alwaysOnTopAct->setCheckable(true);
  alwaysOnTopAct->setStatusTip(tr("Toggle the window to always stay on the top"));
  connect(alwaysOnTopAct, SIGNAL(triggered()), this, SLOT(alwaysOnTop()));

  settingsAct = new QAction(QIcon(":/mainwindow/settings.png"), tr("&Preferences..."), this);
  settingsAct->setStatusTip(tr("Change mClient settings"));
  connect(settingsAct, SIGNAL(triggered()), _mainWindow, SLOT(changeConfiguration()) );

  mumeHelpAct = new QAction(tr("M&ume"), this);
  mumeHelpAct->setStatusTip(tr("MUME Website"));

  wikiAct = new QAction(tr("&Wiki"), this);
  wikiAct->setStatusTip(tr("Visit the MUME Wiki"));

  forumAct = new QAction(tr("&Forum"), this);
  forumAct->setStatusTip(tr("Visit the MUME Forum"));

  clientHelpAct = new QAction(QIcon(":/mainwindow/help.png"), tr("mClient &Help"), this);
  clientHelpAct->setStatusTip(tr("View the mClient/Powwow help file"));
  connect(clientHelpAct, SIGNAL(triggered()), this, SLOT(clientHelp()));

  aboutAct = new QAction(QIcon(":/mainwindow/m.png"), tr("About &mClient"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAct = new QAction(QIcon(":/mainwindow/qt.png"), tr("About &Qt"), this);
  aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

  cutAct->setEnabled(false);
  copyAct->setEnabled(false);
//   connect(ClientManager::self()->getTextEdit(), SIGNAL(copyAvailable(bool)),
//           cutAct, SLOT(setEnabled(bool)));
//   connect(ClientManager::self()->getTextEdit(), SIGNAL(copyAvailable(bool)),
//           copyAct, SLOT(setEnabled(bool)));
}


void ActionManager::disableActions(bool value)
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
}


void ActionManager::createMenus() {
  QMenuBar *menuBar = _mainWindow->menuBar();

  fileMenu = menuBar->addMenu(tr("&File"));
  fileMenu->addAction(connectAct);
  fileMenu->addAction(disconnectAct);
  fileMenu->addAction(reconnectAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  editMenu = menuBar->addMenu(tr("&Edit"));
  editMenu->addAction(cutAct);
  editMenu->addAction(copyAct);
  editMenu->addAction(pasteAct);

  viewMenu = menuBar->addMenu(tr("&View"));
  viewMenu->addAction(alwaysOnTopAct);

  settingsMenu = menuBar->addMenu(tr("&Settings"));
  settingsMenu->addAction(settingsAct);

  menuBar->addSeparator();

  helpMenu = menuBar->addMenu(tr("&Help"));
  helpMenu->addAction(mumeHelpAct);
  helpMenu->addAction(forumAct);
  helpMenu->addAction(wikiAct);
  helpMenu->addSeparator();
  helpMenu->addAction(clientHelpAct);
  helpMenu->addSeparator();
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);

}


void ActionManager::createToolBars() {
  connectToolBar = _mainWindow->addToolBar(tr("Connection"));
  connectToolBar->setObjectName("ToolBarConnect");
  connectToolBar->addAction(connectAct);
  connectToolBar->addAction(disconnectAct);
  connectToolBar->addAction(reconnectAct);
  connectToolBar->setVisible(false);

  editToolBar = _mainWindow->addToolBar(tr("Edit"));
  editToolBar->setObjectName("ToolBarEdit");
  editToolBar->addAction(cutAct);
  editToolBar->addAction(copyAct);
  editToolBar->addAction(pasteAct);
  editToolBar->setVisible(false);
}


void ActionManager::createStatusBar()
{
  _mainWindow->statusBar()->showMessage(tr("Ready"));
}


void ActionManager::alwaysOnTop() {
  _mainWindow->setWindowFlags(_mainWindow->windowFlags() ^ Qt::WindowStaysOnTopHint);
  _mainWindow->show();
}


void ActionManager::about() {
  QString version = 
#ifdef SVN_REVISION
    tr("<b>Subversion Revision ") + QString::number(SVN_REVISION)
#else
#ifdef MCLIENT_VERSION
    tr("<b>mClient Release ") + QString(MCLIENT_VERSION)
#else
    tr("<b>Unknown Release")
#endif
#endif
    + tr("</b><br><br>");
  QMessageBox::about(_mainWindow, tr("About mClient"),
                     tr("<FONT SIZE=\"+1\"><B>mClient ") +
		     version +
		     tr("</B></FONT><P>"
			"Copyright \251 2008 by Jahara<P>"
			"Visit the <A HREF=\"http://code.google.com/p/mclient-mume/\">mClient website</A> "
			"for more information."));
}

void ActionManager::clientHelp() {
  if (!QDesktopServices::openUrl(QUrl::fromEncoded("http://mume.org/wiki/index.php/mClient_Help")))
    qWarning() << "Failed to open web browser";
}

void ActionManager::connectSession() {
  QVariant *payload = new QVariant();
  QStringList tags("ConnectToHost");
  postEvent(payload, tags);
}

void ActionManager::disconnectSession() {
  QVariant *payload = new QVariant();
  QStringList tags("DisconnectFromHost");
  postEvent(payload, tags);  
}

void ActionManager::reconnectSession() {
  disconnectSession();
  connectSession();
}

void ActionManager::postEvent(QVariant *payload, const QStringList& tags) {
  // TODO: Replace with EngineEvents
  QString session = _mainWindow->session();
  PluginSession *ps
    = _mainWindow->getPluginManager()->getPluginSession(session);
  
  MClientEvent *me
    = new MClientEvent(new MClientEventData(payload, tags, session));
  QCoreApplication::postEvent(ps, me);
}
