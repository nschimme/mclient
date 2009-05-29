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

ActionManager *ActionManager::_self = 0;

ActionManager *ActionManager::self(MainWindow *parent)
{
  if (_self == 0) {
    _self = new ActionManager(parent);
    qDebug("Action Manager created.");
  }
  return _self;
}

ActionManager::ActionManager(MainWindow *parent) {
  _mainWindow = parent;
  createActions();
}

ActionManager::~ActionManager() {
  _self = 0;
}

void ActionManager::createActions() {
  connectAct = new QAction(QIcon(":/mainwindow/connect.png"), tr("&Connect..."), this);
  connectAct->setStatusTip(tr("Load a new session and connect to the remote host"));
  //connect(connectAct, SIGNAL(triggered()), _mainWindow, SLOT(selectProfile() ));
  
  disconnectAct = new QAction(QIcon(":/mainwindow/disconnect.png"), tr("&Disconnect"), this);
  disconnectAct->setStatusTip(tr("Disconnect from the current session"));
  //connect(disconnectAct, SIGNAL(triggered()), PowwowWrapper::self(), SLOT(disconnectSession()));

  reconnectAct = new QAction(QIcon(":/mainwindow/reconnect.png"), tr("&Reconnect"), this);
  reconnectAct->setStatusTip(tr("Reconnect to the current session's remote host"));
  //connect(reconnectAct, SIGNAL(triggered()), PowwowWrapper::self(), SLOT(connectSession()) );

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

void ActionManager::alwaysOnTop()
{
  _mainWindow->setWindowFlags(_mainWindow->windowFlags() ^ Qt::WindowStaysOnTopHint);
  _mainWindow->show();
}

void ActionManager::about()
{
#ifdef SVN_REVISION
QString version = tr("<b>Subversion Revision ") + QString::number(SVN_REVISION) + tr("</b><br><br>");
#else
 QString version = tr("<b>mClient Release ") + QString(MCLIENT_VERSION)+ tr("</b><br><br>");
#endif
  QMessageBox::about(_mainWindow, tr("About mClient"),
                     tr("<FONT SIZE=\"+1\"><B>mClient ") + version + tr("</B></FONT><P>"
                         "Copyright \251 2008 Jahara<P>"
                         "Visit the <A HREF=\"http://code.google.com/p/mclient-mume/\">mClient website</A> "
                         "for more information."));
}

void ActionManager::clientHelp()
{
  if (!QDesktopServices::openUrl(QUrl::fromEncoded("http://mume.org/wiki/index.php/mClient_Help")))
    qDebug("Failed to open web browser");
}
