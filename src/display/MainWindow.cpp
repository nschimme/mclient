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
#include <QTextStream>
#include <QCloseEvent>
#include <QTabWidget>
#include <QSplitter>

#include "MainWindow.h"
#include "WindowActionManager.h"
#include "ConfigManager.h"

#include "QuickConnectDialog.h"
#include "ProfileManagerDialog.h"
//#include "AliasEditorDialog.h"

#include "InputWidget.h"
#include "DisplayWebKitWidget.h"

MainWindow::MainWindow(ConfigManager* cfg) {
    _cfg = cfg;
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
    initDisplay("test");

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
    //emit startSession(profile);
}


void MainWindow::initDisplay(const QString& session) {
    // Create the layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    // Add the primary widgets to the smart splitter
    QSplitter *_splitter = new QSplitter(Qt::Vertical);
    _tabWidget->addTab(_splitter, _currentProfile);

    _display = new DisplayWebKitWidget(this);
    _input = new InputWidget(this);

    _splitter->addWidget(_display);
    _splitter->setCollapsible(_splitter->indexOf(_display), false);

    _splitter->addWidget(_input);
    _splitter->setCollapsible(_splitter->indexOf(_input), false);

    /*
      // Display the widget if it floats (or is unsupported)
      QDockWidget *dockWidget = new QDockWidget(this);
      dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
      dockWidget->setFeatures(QDockWidget::DockWidgetMovable
                  | QDockWidget::DockWidgetFloatable
                  );
      dockWidget->setWidget(widgetList[i].second);
      dockWidget->setFloating(false);
      addDockWidget(Qt::RightDockWidgetArea, dockWidget);
      _dockWidgets.insert("test", dockWidget);
      //widgetList.at(i).second->show();
    */

    // Display Main Window
    qDebug() << "* Displaying Main Window";
    show();

    // Connect the signals/slots
    _display->setFocusProxy(_input);
    _input->setFocus();

    connect(_actMgr->copyAct, SIGNAL(triggered()),
            _display, SLOT(copy()));
    connect(_actMgr->pasteAct, SIGNAL(triggered()),
            _input, SLOT(paste()));
    connect(_display, SIGNAL(copyAvailable(bool)),
            _actMgr->cutAct, SLOT(setEnabled(bool)));
    connect(_display, SIGNAL(copyAvailable(bool)),
            _actMgr->copyAct, SLOT(setEnabled(bool)));
    connect(_actMgr->cutAct, SIGNAL(triggered()),
            _display, SLOT(cut()));

    //emit doneLoading();

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
